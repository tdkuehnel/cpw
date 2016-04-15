#ifndef CPW_ARG
#define CPW_ARG

/* from main config or command line arguments. */
typedef struct cpwarguments
{
  char *args[2];                /* arg1 & arg2 */
  int silent, verbose;
  char *config_file;
  int log_level;
} cpwarguments;

void cpw_arg_parse(cpwarguments *arguments, int argc, char **argv);

#endif
