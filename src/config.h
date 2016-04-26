#ifndef CPW_CONFIG_H
#define CPW_CONFIG_H

#include <stdio.h>
#include "context.h"

#define CPW_CONFIG_MAX_LINE_LENGTH 1024
#define CPW_CONFIG_TAG_MAX_ARG_ALLOWED 32
#define CPW_CONFIG_MAX_ERROR_MESSAGE_LENGTH 256

typedef void (*cpw_process_config_func)(void *pointer );

typedef struct cpwconfigtag {
  const char tag[CPW_CONFIG_MAX_TAG_LENGTH];
  const char *argsallowed[CPW_CONFIG_TAG_MAX_ARG_ALLOWED];
  cpw_process_config_func *func;
} cpwconfigtag;

typedef struct cpwconfigerror {
  int line_num;
  char *error_message;
  struct cpwconfigerror *next;
} cpwconfigerror;

typedef struct cpwparsecontext {
  char *configfile_path;
  FILE *stream;
  int in_tag ;
  int line_num;
  char line[CPW_CONFIG_MAX_LINE_LENGTH];
  char current_tag[CPW_CONFIG_MAX_TAG_LENGTH];
  char closing_tag[CPW_CONFIG_MAX_TAG_LENGTH];
  cpwlinetoken *linetoken;
  const char *p;
  cpwconfigerror *configerror;  
} cpwparsecontext;

typedef struct cpwconfig {
  cpwparsecontext *parsecontext;
  struct cpwcommand *command;
  struct cpwprocess *process;
} cpwconfig;

cpwparsecontext *cpw_parsecontext_new();
int cpw_parsecontext_init(cpwparsecontext *parsecontext, const char* configfile_path);
void cpw_parsecontext_done(cpwparsecontext **pparsecontext);

cpwconfig *cpw_config_new();
int cpw_config_init(struct cpwconfig *config, const char *config_file);
int cpw_config_validate(cpwconfig *config);
int cpw_config_parse(cpwconfig *config);
void cpw_config_printout(cpwconfig *config);
void cpw_config_parse_configfile(struct cpwconfig *config);
int cpw_config_validate_configfile(cpwconfig *config);

#endif
