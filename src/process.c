#include <stdio.h>
#include <errno.h>

#include "process.h"
#include "utlist.h"

cpwprocess *cpw_process_new() {
  cpwprocess *process;
  process = calloc(1, sizeof(cpwprocess));
  if (process == NULL)
    return NULL;
  return process;
}

void cpw_process_init(cpwprocess *process) {
  process->pid = 0;
  process->source = NULL;
  process->output = NULL;
  return;
}

int cpw_process_add_source(cpwsource *source, cpwprocess *process) {
  if (source) {
    if (process) {
      DL_APPEND(process->source, source);
      return 0;
    }
  }
  return -1;
}

int cpw_process_add_output(cpwoutput *output, cpwprocess *process) {
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
