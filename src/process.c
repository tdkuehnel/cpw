#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "process.h"
#include "log.h"
#include "utlist.h"

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
  process->pid = 0;
  process->source = NULL;
  process->output = NULL;
  for ( i=0; i<NELEMS(process->args); i++ ) {
    process->args[i] = NULL;
  }
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

int cpw_process_add_arg(cpwprocess *process, const char *arg) {
  int i = 0;
  while (process->args[i] != NULL && i <NELEMS(process->args)) {
    i++;
  }
  if ( i == NELEMS(process->args) ) {
    CPW_LOG_ERROR( "cannot add arg to process, maximum %d reached.\n", CPW_PROCESS_MAX_ARGS);
    return 1;
  }
  process->args[i] = arg;
  return 0;
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

int cpw_process_set_value(cpwprocess *process, const char *cmd, const char *value) {
  if ( strcasecmp("arg", cmd) == 0) {
    return cpw_process_add_arg(process, value);
  } else {
    CPW_LOG_ERROR( "Invalid cmd for cpw_process_set_value: %s\n", cmd);
    return 1;
  }
}

void cpw_process_free(cpwprocess *process) {
  cpwprocess *eprocess, *tprocess;
  cpwsource *esource, *tsource;
  cpwoutput *eoutput, *toutput;
  if (process) {
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
    if (process->args)
      free(process->args);    
    free(process);
  }
}
