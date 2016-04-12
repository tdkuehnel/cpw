#include <argp.h>
#include "arg.h"
#include "config.h"

#define DEBUG 0
#include "debug.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

/* Program documentation. */
static char doc[] =
  "cpw -- remote controlled packetized pipe wonder";

/* A description of the arguments we accept. */
static char args_doc[] = "[OPTION...]";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"quiet",    'q', 0,      0,  "Don't produce any output" },
  {"silent",   's', 0,      OPTION_ALIAS },
  {"configfile",   'f', "FILE", 0, "use configfile instead of ..." },
  { 0 }
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  cpwarguments *arguments = state->input;

  switch (key)
    {
    case 'q': case 's':
      arguments->silent = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'f':
      arguments->config_file = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
        /* Too many arguments. */
        argp_usage (state);

      arguments->args[state->arg_num] = arg;

      break;

    case ARGP_KEY_END:
      if (state->arg_num > 0)
        /* Too many arguments. */
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


void cpw_arg_parse(cpwarguments *arguments, int argc, char **argv) {

  /* fill in default values */
  arguments->config_file = "/etc/cpw.conf";

  argp_parse (&argp, argc, argv, 0, 0, arguments);


  debug_printf ("OUTPUT_FILE = %s\n"
          "VERBOSE = %s\nSILENT = %s\n",
          arguments->config_file,
          arguments->verbose ? "yes" : "no",
          arguments->silent ? "yes" : "no");
}
