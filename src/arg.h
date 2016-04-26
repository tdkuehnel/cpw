#ifndef _CPW_ARG_
#define _CPW_ARG_

/* from main config or command line arguments. */
typedef struct cpwarguments
{
  char *args[2];                /* arg1 & arg2 */
  int silent, verbose;
  char *config_file;
  int log_level;
  int check_config_only;
} cpwarguments;

void cpw_arg_parse(cpwarguments *arguments, int argc, char **argv);

#endif
