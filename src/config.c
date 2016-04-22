#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "utlist.h"
#include "context.h"

#define DEBUG 0
#define DEBUG_DEEP 0
#include "debug.h"


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

const char *tags[] = {
  "Command",
  "Job",
  "Process",
  NULL
};

cpwconfigtag configtags[] = {
  {"Command", {"name", "path", "arg",  NULL}},
  {"Process", {"name", "Command", "arg", "immediate", NULL}},
  {"Job", {"name", NULL}}
};

int cpw_is_space(int c) {
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

int cpw_is_tag(const char *tag) {
  if ( *tag == '<' && tag[1] != '>' && tag[1] != '\0' && tag[1] != ' ') 
    return 1;
  else 
    return 0;
}

int cpw_is_closing_tag(const char *arg) {
  if ( arg[0] == '<' && arg[1] == '/' )
    return 1;
  else
    return 0;
}

int cpw_is_opening_tag(const char *arg) {
  if ( arg[0] == '<' && arg[1] != '/') 
    return 1;
  else
    return 0;
}

void cpw_get_arg(char *buf, int buf_size, const char **pp) {
    const char *p;
    char *q;
    int quote = 0;

    p = *pp;
    q = buf;

    while (cpw_is_space(*p)) p++;

    if (*p == '\"' || *p == '\'')
        quote = *p++;

    while (*p != '\0') {
        if (quote && *p == quote || !quote && cpw_is_space(*p))
            break;
        if ((q - buf) < buf_size - 1)
            *q++ = *p;
        p++;
    }

    *q = '\0';
    if (quote && *p == quote)
        p++;
    *pp = p;
}

void cpw_get_tag(char *buf, int buf_size, char **pp) {
  const char *p;
  char *q;
  
  p = *pp;
  q = buf;
  
  while (*p != '\0') {
    if ( *p == '<' || *p == '/' || *p == '>' ) {
      p++;
    } else {
      if ((q - buf) < buf_size - 1)
	*q++ = *p;
      p++;
    }
  }    
}

int cpw_tag_is_arg_allowed( cpwconfigtag *configtags, int array_size, const char *tag, const char *arg ) {
  int i, j;
  int found = 0;

  /* First we need to find our tag */
  for ( j=0; j<array_size; j++) {
    if ( strcasecmp(tag, configtags[j].tag) == 0 )
      break;
  }
  if ( j == array_size ) {
    CPW_LOG_ERROR("searching for unknown tag <%s> in known tags\n", tag);
    return 0;
  }
  
  CPW_DEBUG("scanning allowed args\n");
  found = 0;
  i = 0;
  while ( configtags[j].argsallowed[i] != NULL ) {
    CPW_DEBUG("scanning arg '%s'\n", configtags[j].argsallowed[i]);
    if ( strcasecmp(configtags[j].argsallowed[i], arg) == 0 ) {
      found = 1;
      break;
    }
    i++;
  }
  return found;
}

#undef DEBUG 
#define DEBUG 0

void cpw_split_line( const char *line, cpwlinetoken *linetoken) {
  const char *p;
  int i;

  p = line;
  linetoken->num=0;

  CPW_DEBUG("\n  split line: %s", line);

  for ( i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++ ) {
    *linetoken->token[i] = '\0';
  }
  for ( i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++ ) {
    while (cpw_is_space(*p))
      p++;
    if (*p == '\0' || *p == '#')
      break;
    cpw_get_arg(linetoken->token[linetoken->num], CPW_CONFIG_MAX_TAG_LENGTH, &p);
    CPW_DEBUG(" found token: %s\n", linetoken->token[i]);
    if ( linetoken->token[i] == '\0' )
      break;
    linetoken->num++;
  }
}

#undef DEBUG 
#define DEBUG 0

int cpw_get_tag_name_index(cpwlinetoken *linetoken) {
  int i = 0;
  int j = 0;
  int found = 0;
  
  if ( linetoken ) {
    if ( linetoken->is_opening_tag ) {
      while ( *linetoken->token[i] != '\0' && i < CPW_CONFIG_MAX_LINE_TOKEN ) {
	CPW_DEBUG("scanning token '%s' for '>' \n", linetoken->token[i]);
	j = 0;
	while ( linetoken->token[i][j] != '\0' ) {
	  if ( linetoken->token[i][j] == '>' ) {
	    CPW_DEBUG("   found token '>' in '%s' \n", linetoken->token[i]);
	    if ( j == 0 && i >= 2 ) 
	      i--;
	    return i;
	  }
	  j++;
	} 
	i++;
      }
    } else {
      CPW_LOG_ERROR("invalid linetoken argument\n");
      return 0;
    }
  } else {
    CPW_LOG_ERROR("invalid linetoken argument\n");
    return 0;
  }
  return i;
}

#undef DEBUG 
#define DEBUG 0

cpwparsecontext *cpw_parsecontext_new() {
  cpwparsecontext *parsecontext;

  parsecontext = calloc(1, sizeof(cpwparsecontext));
  if ( parsecontext ) {
  } else {
    CPW_LOG_ERROR("no mem for cpwprsecontext\n");
    return NULL;
  }
  return parsecontext;
}

int cpw_parsecontext_init(cpwparsecontext *parsecontext, const char* configfile_path) {
  int i;

  if ( parsecontext ) {
    parsecontext->configfile_path = malloc(strlen(configfile_path +1));
    if ( parsecontext->configfile_path ) {
      strcpy(parsecontext->configfile_path, configfile_path);
      parsecontext->stream = fopen(configfile_path, "r");
      if ( parsecontext->stream != NULL) {
	parsecontext->in_tag = 0;
	parsecontext->linetoken = calloc(1, sizeof(cpwlinetoken));
	if ( parsecontext->linetoken ) {
	  for (i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++) {
	    parsecontext->linetoken->token[i] = malloc(CPW_CONFIG_MAX_TAG_LENGTH);
	  }
	  parsecontext->linetoken->is_tag = 0;
	  parsecontext->linetoken->tag_name_index = 0;
	  parsecontext->linetoken->is_opening_tag = 0;
	  parsecontext->linetoken->is_closing_tag = 0;
	} else {
	  CPW_LOG_ERROR("no mem for cpwlinetoken\n");
	  return 0;
	}
	parsecontext->configerror = NULL;
      } else {
	CPW_LOG_ERROR( "Could not open the configuration file '%s'\n", configfile_path);
	return 0;
      }    
    } else {
      CPW_LOG_ERROR("no mem for configfile_path\n");
      return 0;
    }
  } else {
    CPW_LOG_ERROR("Invalid parsecontext\n");
    return 0;
  }
  return 1;
}

#undef DEBUG 
#define DEBUG 0

int cpw_parsecontext_seek_to_tag(cpwparsecontext *parsecontext, const char *tag) {
  int found = 0;

  if ( parsecontext ) {
    while ( cpw_parsecontext_next_token(parsecontext) ) {
      CPW_DEBUG("linetoken[0]: '%s'\n", parsecontext->linetoken->token[0]);
      if ( parsecontext->linetoken->is_tag && parsecontext->linetoken->is_opening_tag ) {
	cpw_get_tag(parsecontext->current_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
	if ( strcasecmp(parsecontext->current_tag, tag) == 0  ) {
	  found = 1;
	  break;
	}
      } else {
	continue;
      }
    }
  } else {
    CPW_LOG_ERROR("Invalid parsecontext\n");
    return 0;
  }
  if (found)
    CPW_DEBUG("seek to token: %s\n", tag); 
  else
    CPW_DEBUG("unable to seek to token: %s\n", tag);   
  return found;
}

#undef DEBUG
#define DEBUG 0

int cpw_parsecontext_next_token(cpwparsecontext *parsecontext) {
  char tag[CPW_CONFIG_MAX_TAG_LENGTH];
  int token_found = 0;
  if ( parsecontext ) {
    while ( fgets(parsecontext->line, sizeof(parsecontext->line), parsecontext->stream) != NULL ) {
      parsecontext->line_num++;
      parsecontext->p = parsecontext->line;
      while ( cpw_is_space(*parsecontext->p) )
        parsecontext->p++;
      if ( *parsecontext->p == '\0' || *parsecontext->p == '#' )
        continue;
      token_found = 1;
      cpw_split_line(parsecontext->p, parsecontext->linetoken);
      CPW_DEBUG("linetoken[0]: '%s'\n", parsecontext->linetoken->token[0]);
      CPW_DEBUG("linetoken[1]: '%s'\n", parsecontext->linetoken->token[1]);
      CPW_DEBUG("linetoken[2]: '%s'\n", parsecontext->linetoken->token[2]);
      if ( cpw_is_tag(parsecontext->linetoken->token[0]) ) { 
	parsecontext->linetoken->is_tag = 1;
	if ( cpw_is_opening_tag(parsecontext->linetoken->token[0]) ) {
	  parsecontext->linetoken->is_closing_tag = 0;
	  parsecontext->linetoken->is_opening_tag = 1;
	  parsecontext->linetoken->tag_name_index = cpw_get_tag_name_index(parsecontext->linetoken);
	  CPW_DEBUG("tag_name_index: %d\n", parsecontext->linetoken->tag_name_index);
	}
	if ( cpw_is_closing_tag(parsecontext->linetoken->token[0]) ) {
	  parsecontext->linetoken->is_closing_tag = 1;
	  parsecontext->linetoken->is_opening_tag = 0;
	  parsecontext->linetoken->tag_name_index = 0;
	}
      } else {
	parsecontext->linetoken->is_tag = 0;
	parsecontext->linetoken->is_opening_tag = 0;
	parsecontext->linetoken->is_closing_tag = 0;
	parsecontext->linetoken->tag_name_index = 0;
      }
      CPW_DEBUG_DEEP("linetoken->is_tag: '%d'\n", parsecontext->linetoken->is_tag);
      CPW_DEBUG_DEEP("linetoken->is_opening_tag: '%d'\n", parsecontext->linetoken->is_opening_tag);
      CPW_DEBUG_DEEP("linetoken->is_closing_tag: '%d'\n", parsecontext->linetoken->is_closing_tag);
      break;
    }
  } else {
    CPW_LOG_ERROR("Invalid parsecontext\n");
    return 0;
  }
  return token_found;
}

void cpw_parsecontext_print_config_error(cpwparsecontext *parsecontext) {
  cpwconfigerror *configerror;
  cpwconfigerror *tmp;
  int errcount;
  int count = 0;
  char errnum[CPW_CONFIG_MAX_TAG_LENGTH];

  if ( parsecontext->configerror ) {
    LL_COUNT(parsecontext->configerror, configerror, errcount);
    snprintf(errnum, CPW_CONFIG_MAX_TAG_LENGTH, "%d", errcount );
    errcount = strlen(errnum);
    
    LL_FOREACH_SAFE(parsecontext->configerror, configerror , tmp) {
      if ( count == 0 ) {
	CPW_LOG_INFO("at line %*d: %s\n", count, configerror->line_num, configerror->error_message);
      } else {
	CPW_LOG_INFO("        %*d: %s\n", count, configerror->line_num, configerror->error_message);
      }
      count ++;
    }  
  }
}

void cpw_parsecontext_add_config_error(cpwparsecontext *parsecontext, const char *fmt, ...) {
  cpwconfigerror *configerror;
  va_list vl;

  if ( parsecontext ) {
    configerror = calloc(1, sizeof(cpwconfigerror));
    if ( configerror ) {
      configerror->error_message = malloc(CPW_CONFIG_MAX_ERROR_MESSAGE_LENGTH);
      if ( configerror->error_message ) {
	va_start(vl, fmt);
	vsnprintf(configerror->error_message, CPW_CONFIG_MAX_ERROR_MESSAGE_LENGTH, fmt, vl);
	va_end(vl);
	LL_APPEND(parsecontext->configerror, configerror);
	configerror->line_num = parsecontext->line_num;
      } else {
	CPW_LOG_ERROR("no mem for error_message\n");
      }  
    } else {
      CPW_LOG_ERROR("no mem for cpwconfigerror\n");
    }  
  } else {
    CPW_LOG_ERROR("Invalid parsecontext\n");
  }  
}

void cpw_parsecontext_done(cpwparsecontext **pparsecontext) {
  int i;
  cpwparsecontext *parsecontext;
  cpwconfigerror *element, *tmp;
  
  if ( pparsecontext ) {
    parsecontext = *pparsecontext;
    if ( pparsecontext ) {
      fclose(parsecontext->stream);
      for ( i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++ ) {
	if ( parsecontext->linetoken->token[i] )	
	  free( parsecontext->linetoken->token[i] );
      }	
      LL_FOREACH_SAFE(parsecontext->configerror, element , tmp) {
	LL_DELETE(parsecontext->configerror, element);
	free( element );
      }    
      if ( parsecontext->configfile_path )
	free(parsecontext->configfile_path);
      free( parsecontext );
      *pparsecontext = NULL;
    } else {
      CPW_LOG_ERROR("Invalid parsecontext\n");
    }
  } else {
    CPW_LOG_ERROR("Invalid argument (*parsecontext)\n");
  }  
}

#undef DEBUG 
#define DEBUG 1

int cpw_config_validate_configfile_logic(cpwparsecontext *parsecontext) {
  cpwcommand *command = NULL;
  
  CPW_LOG_INFO("Validating config file logic %s ...\n", parsecontext->configfile_path);

  rewind(parsecontext->stream);  
  while ( cpw_parsecontext_seek_to_tag(parsecontext, "Command") ) {
    CPW_DEBUG("found tag Command at line: %d\n", parsecontext->line_num);    
    command = cpw_command_new();
    if ( parsecontext->linetoken->tag_name_index ) {
      cpw_command_set_value(command, "name", parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index]);      
    }
    while ( cpw_parsecontext_next_token(parsecontext) && ! parsecontext->linetoken->is_tag ) {
      cpw_command_set_value(command, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);
      CPW_DEBUG("Command %s: setting '%s' to value '%s'\n", command->name, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);    
    }
  }
  rewind(parsecontext->stream);
  while ( cpw_parsecontext_seek_to_tag(parsecontext, "Process") ) {
    CPW_DEBUG("found tag Process at line: %d\n", parsecontext->line_num);    
    
  }
  return 1;
}

#undef DEBUG 
#define DEBUG 0

int cpw_config_validate_configfile_syntax(cpwparsecontext *parsecontext) {
  
  CPW_LOG_INFO("Validating config file syntax %s ...\n", parsecontext->configfile_path);
  rewind(parsecontext->stream);
      
  while ( cpw_parsecontext_next_token(parsecontext) )
    {
      if ( parsecontext->linetoken->is_opening_tag ) {
	CPW_DEBUG(" opening tag: %s\n", parsecontext->linetoken->token[0]);
	if ( parsecontext->in_tag ) {	      
	  cpw_parsecontext_add_config_error(parsecontext, "tag '%s' in tag '<%s>', closing tag missing", 
					    parsecontext->linetoken->token[0], parsecontext->current_tag);	  
	} else {
	  parsecontext->in_tag = 1;
	  cpw_get_tag(parsecontext->current_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
	}
      } else if ( parsecontext->linetoken->is_closing_tag ) {
	CPW_DEBUG(" closing tag: %s\n", parsecontext->linetoken->token[0]);
	if ( ! parsecontext->in_tag ) {
	  cpw_parsecontext_add_config_error(parsecontext, "dangling closing tag '%s'", 
					    parsecontext->linetoken->token[0]);	  
	} else {
	  cpw_get_tag(parsecontext->closing_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
	  if ( strcasecmp(parsecontext->current_tag, parsecontext->closing_tag) == 0 ) {
	    CPW_DEBUG("Found closing tag for: <%s>\n", parsecontext->current_tag);
	    parsecontext->in_tag = 0;
	    *parsecontext->current_tag = '\0';
	  } else {
	    cpw_parsecontext_add_config_error(parsecontext, "closing tag mismatch: '<%s>' <> '%s'", 
					      parsecontext->current_tag, parsecontext->linetoken->token[0]);	  
	  }
	} 
      } else {
	CPW_DEBUG("      no tag: %s\n", parsecontext->linetoken->token[0]);
	if ( parsecontext->in_tag ) {
	  if ( cpw_tag_is_arg_allowed( configtags, NELEMS(configtags), parsecontext->current_tag, parsecontext->linetoken->token[0]) ) {
	    CPW_DEBUG("allowed arg '%s' for tag <%s>\n", parsecontext->linetoken->token[0],parsecontext-> current_tag);
	  } else {
	    CPW_DEBUG("arg '%s' not allowed for tag <%s>\n", parsecontext->linetoken->token[0],parsecontext-> current_tag);
	    cpw_parsecontext_add_config_error(parsecontext, "arg '%s' not allowed for tag '<%s>'", 
					      parsecontext->linetoken->token[0], parsecontext->current_tag);	    
	  }
	} else {	  
	}
      }
    }
  if ( parsecontext->in_tag )
    cpw_parsecontext_add_config_error(parsecontext, "(at end of file) Missing closing tag for tag '<%s>'", parsecontext->current_tag);	      
  if ( parsecontext->configerror )
    return 0;
  else
    return 1;
}  

cpwcommand *cpw_config_parse_config_for_command(cpwconfig *config) {
}

void cpw_config_parse_configfile(cpwconfig *config) {
  cpwcommand *command;
  if ( config) {
    command = cpw_config_parse_config_for_command(config);
  } else {
    CPW_LOG_ERROR("Invalid context argument\n");
  }
}

int cpw_config_parse(cpwconfig *config) {
  cpw_config_parse_configfile(config);
}

int cpw_config_validate(cpwconfig *config) {
  if ( cpw_config_validate_configfile_syntax(config->parsecontext) ) {
    CPW_LOG_INFO("configfile syntax looks OK\n");
    if ( cpw_config_validate_configfile_logic(config->parsecontext) ) {
      CPW_LOG_INFO("configfile logic looks OK\n");
    } else {
      CPW_LOG_ERROR("Invalid config file logic: %s\n", config->parsecontext->configfile_path);
      cpw_parsecontext_print_config_error(config->parsecontext);
      return 0;
    }
  } else {
    CPW_LOG_ERROR("Invalid config file syntax: %s\n", config->parsecontext->configfile_path);
    cpw_parsecontext_print_config_error(config->parsecontext);
    return 0;
  }
  return 1;
}

cpwconfig *cpw_config_new() {
  cpwconfig *config;
  config = calloc(1, sizeof(cpwconfig));
  if ( config )
    return config;
  else {
    CPW_LOG_ERROR("no mem for cpwconfig\n");    
    return NULL;
  }
}

int cpw_config_init(cpwconfig *config, const char *config_file) {
  if ( config ) {
    config->parsecontext = cpw_parsecontext_new();
    cpw_parsecontext_init(config->parsecontext, config_file);    
    config->command = NULL;
    config->process = NULL;
  } else {
    CPW_LOG_ERROR("Invalid cpwconfig\n");
    return 0;
  }
  return 1;
}

