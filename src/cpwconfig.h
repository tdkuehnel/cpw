#ifndef CPW_CONFIG_H
#define CPW_CONFIG_H

#include "command.h"

typedef struct cpwconfig {
  cpwcommand *command;
}cpwconfig;

int cpw_config_parse(const char *filename, cpwconfig *config);

#endif
