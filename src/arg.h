#ifndef CPW_ARG
#define CPW_ARG

/* Used by main to communicate with parse_opt. */
typedef struct cpwarguments
{
  char *args[2];                /* arg1 & arg2 */
  int silent, verbose;
  char *config_file;
} cpwarguments;

void cpw_arg_parse(cpwarguments *arguments, int argc, char **argv);

#endif
