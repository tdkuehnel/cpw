#ifndef _CPW_COMMANDARG_H_
#define _CPW_COMMANDARG_H_

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

cpwcommandargindex *cpw_commandargindex_new(int index);

cpwcommandarg *cpw_commandarg_new();
void cpw_commandarg_init(cpwcommandarg *commandarg);
void cpw_commandarg_done(cpwcommandarg **pcommandarg);



#endif
