#ifndef _CPW_COMMAND_H_
#define _CPW_COMMAND_H_

#include "commandarg.h"

typedef struct cpwcommand {
  struct cpwcommand *next, *prev;
  char name[CPW_COMMAND_MAX_ARG_LENGTH];
  char path[CPW_COMMAND_MAX_ARG_LENGTH];
  cpwcommandarg *args;
} cpwcommand;

/*int cpw_find_token(const char* line, const char* token, int index);*/

cpwcommand *cpw_command_new();
void cpw_command_init(cpwcommand *command);
int cpw_command_set_path(cpwcommand *command, const char *path);
int cpw_command_set_empty_path(cpwcommand *command, const char *path);
cpwcommand *cpw_command_find_by_name(cpwcommand *commandlist, const char *name);
int cpw_command_set_name(cpwcommand *command, const char *name);
int cpw_command_set_empty_name(cpwcommand *command, const char *name);
int cpw_command_set_value(cpwcommand *command, const char *cmd, const char *value);
void cpw_command_done(cpwcommand **pcommand);

#endif
