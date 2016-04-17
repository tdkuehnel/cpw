#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "context.h"

#define DEBUG 1
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
  {"Command", {"arg", "arg2", "arg4", NULL}},
  {"Process", {"arg", "arg2", "arg4", NULL}},
  {"Job",{NULL}},
};

cpwlinetoken linetoken;

void cpw_config_init_tags() {

}

int cpw_config_init(cpwcontext *context) {
  if ( context ) {
    if ( context->config && context->arguments ) {
      context->config->configfile_path = context->arguments->config_file;
      context->config->line_num = 0;
      if ( cpw_config_validate_configfile(context->config) ) {
	cpw_config_parse_configfile(context->config);
      } else {
	CPW_LOG_ERROR("Invalid config file '%s':\n", context->config->configfile_path);
	return 1;
      }
    } else {
      CPW_LOG_ERROR("Invalid context->config or context->arguments\n");
      return 1;
    }
  } else {
    CPW_LOG_ERROR("Invalid context in cpw_config_init\n");
    return 1;
  }
  return 0;
}

int cpw_is_space(int c) {
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

int cpw_is_tag(const char *tag) {
  if ( *tag == '<' ) 
    return 1;
  else
    return 0;
}

int cpw_is_closing_tag(const char *arg) {
  const char *p;
  if ( arg[1] == '/' )
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

void cpw_config_parse_configfile(cpwconfig *config) {
}

void cpw_split_line( const char *line, cpwlinetoken *linetoken) {
  const char *p;
  int i;

  p = line;
  linetoken->num=0;

  CPW_NO_DEBUG("  split line: %s", line);

  for ( i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++ ) {
    while (cpw_is_space(*p))
      p++;
    if (*p == '\0' || *p == '#')
      break;
    cpw_get_arg(linetoken->token[linetoken->num], CPW_CONFIG_MAX_TAG_LENGTH, &p);
    CPW_LOG_INFO(" found token: %s\n", linetoken->token[i]);
    if ( linetoken->token[i] == '\0' )
      break;
    linetoken->num++;
  }
}

int cpw_config_validate_configfile(cpwconfig *config) {
  FILE *f;
  const char *p;
  char line[CPW_CONFIG_MAX_LINE_LENGTH];
  int i;
  int in_tag = 0;
  char current_tag[CPW_CONFIG_MAX_TAG_LENGTH];


  CPW_LOG_INFO("sizeof(configtags): %d\n", NELEMS(configtags));

  if (config->configfile_path != NULL) {
    f = fopen(config->configfile_path, "r");
    if (f != NULL) {
      CPW_LOG_INFO("Validating config file %s\n", config->configfile_path);
      for (i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++) linetoken.token[i] = malloc(CPW_CONFIG_MAX_TAG_LENGTH);
      
      while (fgets(line, sizeof(line), f) != NULL) {
	config->line_num++;
	p = line;
	while (cpw_is_space(*p))
	  p++;
	if (*p == '\0' || *p == '#')
	  continue;
	CPW_DEBUG("\n        line: %s", p);
	cpw_split_line(p, &linetoken);
	if ( linetoken.num > 0 ) {
	  if ( cpw_is_tag(linetoken.token[0] )) {
	    /* tag or closing tag */
	    CPW_DEBUG("    have tag: %s\n", linetoken.token[0]);
	    if ( ! in_tag ) {	      
	      /* need only opening tag */
	      if ( ! cpw_is_closing_tag(linetoken.token[0]) ) {
		cpw_get_tag(current_tag, CPW_CONFIG_MAX_TAG_LENGTH, &linetoken.token[0]);
		in_tag = 1;		
	      } else {
		CPW_LOG_ERROR( "at line: %d, dangling closing tag '%s'\n", config->line_num, linetoken.token[0]);
		return 1;
	      }
	    } else {
	      /* need only closing tag */
	      if ( cpw_is_closing_tag(linetoken.token[0]) ) {
		in_tag = 0;
	      } else {
		CPW_LOG_ERROR( "at line: %d, tag '%s' in tag '%s'\n", config->line_num, linetoken.token[0], current_tag);
		return 1;
	      }
	    }
	  } else {
	    /* no tag at all, only parameters on line */
	    CPW_DEBUG("      no tag: %s\n", linetoken.token[0]);
	    if ( in_tag ) {
	      /* only parameters for tag <current_tag> allowed */
	      if ( linetoken.num >0 ) {
		if ( cpw_tag_is_arg_allowed( configtags, NELEMS(configtags), current_tag, linetoken.token[0]) ) {
		  CPW_LOG_INFO("allowed arg '%s' for tag <%s>\n", linetoken.token[0], current_tag);
		} else {
		  CPW_LOG_ERROR("arg '%s' not allowed for tag <%s>\n", linetoken.token[0], current_tag);
		  return 1;
		}
	      }
	    } else {
	      /* only global main parameters allowed */
	      if ( linetoken.num >0 ) {
	      }
	    }
	  }
	} else {
	  CPW_LOG_ERROR("trying to operate on empty linetoken\n");
	  return 1;
	}
      }      
    } else {
      CPW_LOG_ERROR( "Could not open the configuration file '%s'\n", config->configfile_path);
      return 1;
    }
  } else {
    CPW_LOG_WARNING("No config file, using MAIN default values. No job or process definitions. \n");    
    return 1;
  }
  if ( in_tag ) {
    CPW_LOG_ERROR( "at line %d (OAend of file): Missing closing tag for tag '%s'\n", current_tag);
    return 1;
  }
  return 0;
}
