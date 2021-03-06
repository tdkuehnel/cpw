#include <argp.h>
#include "arg.h"
#include "autoconfig.h"
#include "log.h"

#define DEBUG 0
#define DEBUG_DEEP 0
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
  /* long name    sn   arg     flags         description*/
  {"Verbose",     'v', 0,      0,            "Produce verbose output" },
  {"quiet",       'q', 0,      0,            "Don't produce any output" },
  {"silent",      's', 0,      OPTION_ALIAS },
  {"configfile",  'f', "FILE", 0,            "Use configfile instead of ..." },
  {"checkconfig", 'c', 0,      0,            "Check only configfile syntax and logic, then exit"},
  {"loglevel",    'l', "LEVEL",0,            "Set log level (0 .. 8), default 1" },
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
    case 'c':
      arguments->check_config_only = 1;
      break;
    case 'f':
      arguments->config_file = arg;
      break;
    case 'l':
      arguments->log_level = atoi(arg);
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
  /* FIXME: take cpw.conf path from configure values */
  arguments->config_file = "/etc/cpw.conf";
  arguments->log_level = CPW_LOG_LEVEL_DEFAULT;
  arguments->check_config_only = 0;

  CPW_DEBUG ("OUTPUT_FILE = %s\n"
		"VERBOSE = %s\nSILENT = %s\nLOG_LEVEL = %d\n\n",
		arguments->config_file,
		arguments->verbose ? "yes" : "no",
		arguments->silent ? "yes" : "no",
		arguments->log_level);

  /* FIXME: argp_parse drops out if help/usage is displayed or wrong params provided. Need proper program cleanup. */
  argp_parse (&argp, argc, argv, 0, 0, arguments);

  CPW_DEBUG ("OUTPUT_FILE = %s\n"
		"VERBOSE = %s\nSILENT = %s\nLOG_LEVEL = %d\n\n",
		arguments->config_file,
		arguments->verbose ? "yes" : "no",
		arguments->silent ? "yes" : "no",
		arguments->log_level);
}
