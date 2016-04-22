#ifndef CPW_COMMAND_H
#define CPW_COMMAND_H

#define CPW_COMMAND_MAX_ARG_LENGTH 256
/*#define CPW_COMMAND_MAX_ARGS        16*/

typedef struct cpwcommandargindex {
  struct cpwcommandargindex *next, *prev;
  int index;
} cpwcommandargindex;

typedef struct cpwcommandarg {
  struct cpwcommandarg *next, *prev;
  char arg[CPW_COMMAND_MAX_ARG_LENGTH];
  cpwcommandargindex *input;  /* List of indexes to '<input>' found in arg */
  cpwcommandargindex *output; /* List of indexes to '<output>' found in arg */
} cpwcommandarg;

typedef struct cpwcommand {
  struct cpwcommand *next, *prev;
  char name[CPW_COMMAND_MAX_ARG_LENGTH];
  char path[CPW_COMMAND_MAX_ARG_LENGTH];
  cpwcommandarg *args;
} cpwcommand;

int cpw_find_token(const char* line, const char* token, int index);

cpwcommandargindex *cpw_commandargindex_new(int index);

cpwcommandarg *cpw_commandarg_new();
void cpw_commandarg_init(cpwcommandarg *commandarg);
void cpw_commandarg_done(cpwcommandarg **pcommandarg);

cpwcommand *cpw_command_new();
void cpw_command_init(cpwcommand *command);
int cpw_command_set_path(cpwcommand *command, const char *path);
int cpw_command_set_empty_path(cpwcommand *command, const char *path);
int cpw_command_set_name(cpwcommand *command, const char *name);
int cpw_command_set_empty_name(cpwcommand *command, const char *name);
int cpw_command_set_value(cpwcommand *command, const char *cmd, const char *value);
void cpw_command_done(cpwcommand **pcommand);

#endif
