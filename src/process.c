#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "process.h"
#include "log.h"
#include "utlist.h"

#include "debug.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

cpwprocess *cpw_process_new() {
  cpwprocess *process;
  process = calloc(1, sizeof(cpwprocess));
  if (process == NULL)
    return NULL;
  cpw_process_init(process);
  return process;
}

void cpw_process_init(cpwprocess *process) {
  int i;
  process->command = NULL;
  process->pid = 0;
  process->source = NULL;
  process->output = NULL;
  process->args = NULL;
  return;
}

int cpw_process_add_source(cpwprocess *process, cpwsource *source) {
  if (source) {
    if (process) {
      DL_APPEND(process->source, source);
      return 0;
    }
  }
  return -1;
}

int cpw_process_add_output(cpwprocess *process, cpwoutput *output) {
  if (output) {
    if (process) {
      DL_APPEND(process->output, output);
      return 0;
    }
  }
  return -1;
}

int cpw_process_create(char *execpath, char *args[CPW_PROCESS_MAX_ARGS]) {
  int r;
  r = fork();
  switch ( r )
    {
    case -1 : 
      printf("fork failed in create_process\n");
      return -1;
      break;
    case 0 : {
      if (freopen("/dev/null", "r", stdin) < 0)
	printf("error redirecting child stdin\n");
      if (freopen("/dev/null", "w", stdout) < 0)
	printf("error redirecting child stdout\n");
      if (freopen("/dev/null", "w", stderr) < 0)
	printf("error redirecting child stderr\n");
      r = execv(execpath, args);
      printf("execv returned with error: %s\n", strerror(errno));
      printf("child process ends now\n");
      /* add proper errno discovery and describe*/
      exit(1);
      break;
    }
    default:
      return r;
      break;
    }
}

#undef DEBUG
#define DEBUG 0

int cpw_process_add_arg(cpwprocess *process, const char *arg) {
  cpwcommandarg *newarg;
  newarg = cpw_commandarg_new();
  if ( newarg ) {
    strncpy(newarg->arg, arg, CPW_COMMAND_MAX_ARG_LENGTH - 1);
    newarg->arg[CPW_COMMAND_MAX_ARG_LENGTH - 1] = '\0';    
    LL_APPEND(process->args, newarg);
    CPW_DEBUG("cpwprocess: adding value '%s' to arglist\n", arg);
    return 1;
  } else {
    CPW_LOG_ERROR("error getting mem for processarg\n");
    return 0;
  }
}

#undef DEBUG
#define DEBUG 0

int cpw_process_set_command(cpwprocess *process, const char *value, void *pointer) {
  cpwcommand *commandlist;
  cpwcommand *command;

  commandlist = pointer;
  if ( commandlist ) {
    CPW_DEBUG("trying to set Process '%s' command to: '%s'\n", process->name, value);
    LL_FOREACH(commandlist, command) {
      if ( strcasecmp(command->name, value) == 0 ) {
	process->command = command;
      } 
    }
    if ( process->command ) {
      CPW_DEBUG("setting Process '%s' command to: '%s'\n", process->name, process->command->name);
      return 1;
    } else {
      CPW_DEBUG("unable to set Process '%s' command to: '%s'\n", process->name, value);
      return 0;
    }
  } else {
    CPW_LOG_ERROR( "Invalid pointer to commandlist for cpw_process_set_command: %p\n", pointer);
    return 0;
  }
}

#undef DEBUG
#define DEBUG 0

int cpw_process_set_immediate(cpwprocess *process, int immediate) {
}

int cpw_process_set_name(cpwprocess *process, const char *name) {
  char uname[CPW_COMMAND_MAX_ARG_LENGTH];
  char *p;

  p = uname;
  while ( *name != '\0' && *name != '>' ) {
    *p++ = *name++;    
  }
  *p = '\0';

  CPW_DEBUG("cpwprocess: setting value 'name' to %s\n", uname);
  strncpy(process->name, uname, CPW_COMMAND_MAX_ARG_LENGTH);
  return 1;
}

#undef DEBUG
#define DEBUG 1

int cpw_process_set_value(cpwprocess *process, const char *arg, const char *value, void *pointer) {
  int i;
  if ( strcasecmp("arg", arg) == 0) {
    return cpw_process_add_arg(process, value);
  } else if ( strcasecmp("command", arg) == 0 ) {
    return cpw_process_set_command(process, value, pointer);
  } else if ( strcasecmp("immediate", arg) == 0 ) {
    return cpw_process_set_immediate(process, atoi(value));
  } else if ( strcasecmp("name", arg) == 0 ) {
    return cpw_process_set_name(process, value);
  } else {
    CPW_DEBUG("invalid arg '%s' in set_value\n", arg);
    CPW_LOG_ERROR( "Invalid cmd for cpw_process_set_value: %s\n", arg);
    return 0;
  }
}

#undef DEBUG
#define DEBUG 0

void cpw_process_free(cpwprocess **pprocess) {
  cpwprocess *eprocess, *tprocess, *process;
  cpwsource *esource, *tsource;
  cpwoutput *eoutput, *toutput;
  cpwcommandarg *element, *tmp;

  if ( pprocess ) {
    process = *pprocess;
    if ( process ) {
      if (process->source) {
	DL_FOREACH_SAFE(process->source, esource, tsource) {
	  DL_DELETE(process->source, esource);
	}
      }
      if (process->output) {
	DL_FOREACH_SAFE(process->output, eoutput, toutput) {
	  DL_DELETE(process->output, eoutput);
	}
      }
      LL_FOREACH_SAFE(process->args, element , tmp) {
	LL_DELETE(process->args, element);
	free( element );
      }    
      *pprocess = NULL;
    } else {
      CPW_LOG_ERROR("Invalid process\n");
    }
  } else {
    CPW_LOG_ERROR("Invalid argument (*pprocess)\n");
  }  
}
