#ifndef CPW_COMMAND_H
#define CPW_COMMAND_H

#define CPW_COMMAND_MAX_LENGTH 256

typedef struct cpwcommand {
  struct cpwcommand *next, *prev;
  const char *uname;
} cpwcommand;

cpwcommand *cpw_command_new();
void cpw_command_init(cpwcommand *command);
int cpw_command_add_path(cpwcommand *command, const char *path);
int cpw_command_set_value(cpwcommand *command, const char *cmd, const char *value);

#endif
