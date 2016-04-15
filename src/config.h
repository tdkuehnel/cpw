#ifndef CPW_CONFIG_H
#define CPW_CONFIG_H

#include <stdio.h>
#include "context.h"

#define CPW_CONFIG_MAX_LINE_LENGTH 1024
#define CPW_CONFIG_MAX_TAG_LENGTH 64

typedef struct cpwconfig {
  int line_num;
  cpwcommand *command;
} cpwconfig;

typedef void (*cpw_process_config_func)(void *pointer );

typedef struct cpwconfigtag {
  const char *tag[CPW_CONFIG_MAX_TAG_LENGTH];
  cpw_process_config_func *func;
} cpwconfigtag;

int cpw_config_init(struct cpwcontext *context);

#endif
