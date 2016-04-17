#ifndef CPW_CONFIG_H
#define CPW_CONFIG_H

#include <stdio.h>
#include "context.h"

#define CPW_CONFIG_MAX_LINE_LENGTH 1024
#define CPW_CONFIG_MAX_TAG_LENGTH 64
#define CPW_CONFIG_TAG_MAX_ARG_ALLOWED 32
#define CPW_CONFIG_MAX_LINE_TOKEN 5

typedef struct cpwconfig {
  char *configfile_path;
  int line_num;
  
  cpwcommand *command;
  int errors;
} cpwconfig;

typedef void (*cpw_process_config_func)(void *pointer );

typedef struct cpwconfigtag {
  const char tag[CPW_CONFIG_MAX_TAG_LENGTH];
  const char *argsallowed[CPW_CONFIG_TAG_MAX_ARG_ALLOWED];
  cpw_process_config_func *func;
} cpwconfigtag;

typedef struct cpwlinetoken {
  /* Actual an array of char * of size MAX_NUM_LINE_TOKEN*/
  char *token[CPW_CONFIG_MAX_LINE_TOKEN];
  int num;
} cpwlinetoken;

int cpw_config_init(struct cpwcontext *context);
void cpw_config_parse_configfile(cpwconfig *config);
int cpw_config_validate_configfile(cpwconfig *config);

#endif
