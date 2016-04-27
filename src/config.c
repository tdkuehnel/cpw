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

cpwparsecontext *cpw_parsecontext_new() {
  cpwparsecontext *parsecontext;

  parsecontext = calloc(1, sizeof(cpwparsecontext));
  if ( parsecontext ) {
    return parsecontext;
  } else {
    CPW_LOG_ERROR("no mem for cpwprsecontext\n");
    return NULL;
  }
}

int cpw_parsecontext_init(cpwparsecontext *parsecontext, const char* configfile_path) {
  int i;

  if ( parsecontext ) {
    parsecontext->configfile_path = malloc(strlen(configfile_path) + 1);
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
	CPW_DEBUG("before cpw_get_tag: parsecontext->linetoken->token[0]: '%p'\n", parsecontext->linetoken->token[0]);
	cpw_get_tag(parsecontext->current_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
	CPW_DEBUG(" after cpw_get_tag: parsecontext->linetoken->token[0]: '%p'\n", parsecontext->linetoken->token[0]);
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
#define DEBUG 0

int cpw_config_validate_configfile_logic(cpwconfig *config, cpwparsecontext *parsecontext) {
  cpwcommand *commandlist = NULL;
  cpwprocess *processlist = NULL;
  cpwcommand *command = NULL;
  cpwprocess *process = NULL;
  char name[CPW_CONFIG_MAX_TAG_LENGTH];

  CPW_LOG_INFO("Validating config file logic %s ...\n", parsecontext->configfile_path);

  rewind(parsecontext->stream);  
  while ( cpw_parsecontext_seek_to_tag(parsecontext, "Command") ) {
    CPW_DEBUG("found tag 'Command' at line: %d\n", parsecontext->line_num);    
    command = cpw_command_new();
    if ( parsecontext->linetoken->tag_name_index ) {
      cpw_get_tag(name, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index]);
      if ( cpw_command_find_by_name(commandlist, name) ) {
   	cpw_parsecontext_add_config_error(parsecontext, "invalid logic: Command with name '%s' already defined", name);	   
      }
      cpw_command_set_value(command, "name", parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index]);      
      CPW_DEBUG("Command   : setting name to value '%s'\n", parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index]);    
    }
    while ( cpw_parsecontext_next_token(parsecontext) && ! parsecontext->linetoken->is_tag ) {
      cpw_command_set_value(command, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);
      CPW_DEBUG("Command %s: setting '%s' to value '%s'\n", command->name, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);    
    }
    CPW_DEBUG("end of while loop for tag: '%s'. now parsecontext->linetoken: '%s'\n", parsecontext->current_tag, parsecontext->linetoken->token[0]);
    CPW_DEBUG("parsecontext->closing_tag = '%s'\n", parsecontext->closing_tag);
    if ( parsecontext->linetoken->is_closing_tag ) {
      cpw_get_tag(parsecontext->closing_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
      CPW_DEBUG("parsecontext->closing_tag = '%s'\n", parsecontext->closing_tag);
      if ( strcasecmp(parsecontext->closing_tag, parsecontext->current_tag) != 0 ) 
	/* should never happen after a syntax check has run on the file, but who knows */ 
	cpw_parsecontext_add_config_error(parsecontext, "closing tag mismatch: '<%s>' <> '%s'", 
					  parsecontext->current_tag, parsecontext->linetoken->token[0]);	  
    }
    LL_APPEND(commandlist, command);
  }
  rewind(parsecontext->stream);
  while ( cpw_parsecontext_seek_to_tag(parsecontext, "Process") ) {
    CPW_DEBUG("found tag Process at line: %d\n", parsecontext->line_num);    
    process = cpw_process_new();
    if ( parsecontext->linetoken->tag_name_index ) {
      cpw_process_set_value(process, "name", parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index], NULL);      
      CPW_DEBUG("Process   : setting name to value '%s'\n", parsecontext->linetoken->token[parsecontext->linetoken->tag_name_index]);    
    }
    while ( cpw_parsecontext_next_token(parsecontext) && ! parsecontext->linetoken->is_tag ) {
      CPW_DEBUG("Process %s: setting '%s' to value '%s'\n", process->name, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);    
      if (! cpw_process_set_value(process, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1], commandlist) ) {
	CPW_DEBUG("Process %s: unable to set '%s' to value '%s'\n", process->name, parsecontext->linetoken->token[0], parsecontext->linetoken->token[1]);
   	cpw_parsecontext_add_config_error(parsecontext, "invalid logic: in Process '%s': '<%s>' not a valid command name (no command with this name)",  
					  process->name, parsecontext->linetoken->token[1]);	   
      }
    }
    LL_APPEND(processlist, process);    
  }
  if ( parsecontext->configerror )
    return 0;
  else {
    if ( config ) {
      config->command = commandlist;
      config->process = processlist;      
    } else {
      CPW_LOG_ERROR("invalid config argument\n");
      return 0;
    }
    return 1;
  }
}

#undef DEBUG 
#define DEBUG 0

int cpw_config_validate_configfile_syntax(cpwconfig *config, cpwparsecontext *parsecontext) {
  
  CPW_LOG_INFO("Validating config file syntax %s ...\n", parsecontext->configfile_path);
  rewind(parsecontext->stream);
      
  while ( cpw_parsecontext_next_token(parsecontext) )
    {
      if ( parsecontext->linetoken->is_opening_tag ) {
	CPW_DEBUG(" opening tag: %s\n", parsecontext->linetoken->token[0]);
	if ( parsecontext->in_tag ) {	      
	  cpw_parsecontext_add_config_error(parsecontext, "tag '%s' in tag '<%s>', closing tag missing", 
					    parsecontext->linetoken->token[0], parsecontext->current_tag);	  
	  cpw_get_tag(parsecontext->current_tag, CPW_CONFIG_MAX_TAG_LENGTH, &parsecontext->linetoken->token[0]);
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
	    CPW_DEBUG("unknown closing tag '%s' for: <%s>\n",parsecontext->closing_tag,  parsecontext->current_tag);
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

void cpw_config_printout(cpwconfig *config) {
  cpwcommand *command;
  cpwprocess *process;
  cpwcommandarg *commandarg;

  int count;

  CPW_LOG_INFO("current configuration: \n");
  if ( config ) {
    if ( config->command ) {
      LL_COUNT(config->command, command, count);      
      CPW_LOG_INFO("%d Command definitions: (%d)\n", count, count);
      count = 1;
      LL_FOREACH(config->command, command) {
	CPW_LOG_INFO("Command(%d):       name: '%s'\n", count, command->name);
	CPW_LOG_INFO("       (%d):       path: '%s'\n", count, command->path);
	CPW_LOG_INFO("       (%d):       args: '", count);
	LL_FOREACH(command->args, commandarg) {
	  CPW_LOG_INFO("%s", commandarg->arg);
	  if ( commandarg->next )
	    CPW_LOG_INFO(" ");
	}
	CPW_LOG_INFO("'\n\n");
	count++;
      }
    } else {
      CPW_LOG_INFO("No Command definitions (0)\n");
    }
    
    if ( config->process ) {
      LL_COUNT(config->process, process, count);      
      CPW_LOG_INFO("%d Process definitions: (%d)\n", count, count);
      count = 1;
      LL_FOREACH(config->process, process) {
	CPW_LOG_INFO("Process(%d):       name: '%s'\n", count, process->name);
	if ( process->command )
	  CPW_LOG_INFO("       (%d):    Command: '%s'\n", count, process->command->name);
	CPW_LOG_INFO("       (%d):       args: '", count);
	LL_FOREACH(process->args, commandarg) {
	  CPW_LOG_INFO("%s", commandarg->arg);
	  if ( commandarg->next )
	    CPW_LOG_INFO(" ");
	}
	CPW_LOG_INFO("'\n\n");
	count++;
      }
    } else {
      CPW_LOG_INFO("No Process definitions (0)\n");
    }

  } else {
    CPW_LOG_ERROR("Invalid config argument\n");
  }
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
  if ( cpw_config_validate_configfile_syntax(config, config->parsecontext) ) {
    CPW_LOG_INFO("configfile syntax looks OK\n");
    if ( cpw_config_validate_configfile_logic(config, config->parsecontext) ) {
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
    config->command = NULL;
    config->process = NULL;
    config->parsecontext = cpw_parsecontext_new();
    if (! cpw_parsecontext_init(config->parsecontext, config_file) ) {
      CPW_LOG_ERROR("unable to initialize parse context\n");
      return 0;
    }        
  } else {
    CPW_LOG_ERROR("Invalid cpwconfig\n");
    return 0;
  }
  return 1;
}

