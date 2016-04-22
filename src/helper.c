#include "helper.h"
#include "log.h"
#include "debug.h"

#undef DEBUG
#define DEBUG 0

int cpw_is_space(int c) {
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

#undef DEBUG
#define DEBUG 0

int cpw_is_tag(const char *tag) {
  if ( *tag == '<' && tag[1] != '>' && tag[1] != '\0' && tag[1] != ' ') 
    return 1;
  else 
    return 0;
}

#undef DEBUG
#define DEBUG 0

int cpw_is_closing_tag(const char *arg) {
  if ( arg[0] == '<' && arg[1] == '/' )
    return 1;
  else
    return 0;
}

#undef DEBUG
#define DEBUG 0

int cpw_is_opening_tag(const char *arg) {
  if ( arg[0] == '<' && arg[1] != '/') 
    return 1;
  else
    return 0;
}

#undef DEBUG
#define DEBUG 0

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

#undef DEBUG
#define DEBUG 0

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

#undef DEBUG
#define DEBUG 0

int cpw_find_token(const char* line, const char* token, int index) {
  const char *p, *q, *l;
  int i = 0;

  if ( line && token ) {
    CPW_DEBUG("line: '%s', token: '%s'\n", line, token);
    l = line;
    while (*l != '\0' ) {
      p = l;
      q = token;
      while ( *p == *q && *p != '\0' ) {
	CPW_DEBUG("%s \n", q);
	p++;
	q++;
      }
      if ( *q == '\0' ) {
	if ( i == index ) 
	  return l - line;
	else
	  i++;
      }
      l++;
    }
    return -1;
  } else {
    CPW_LOG_ERROR("invalid argument(s)\n");
  } 
  return -1;
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

