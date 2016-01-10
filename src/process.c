include "process.h"

cpwprocess *cpw_process_new() {
  cpwprocess *process;
  process = calloc(1, sizeof(cpwprocess));
  if (process == NULL)
    return NULL;
  process->args = calloc(CPW_PROCESS_MAX_ARGS, CPW_PROCESS_ARG_LEN + 1);
  if (process == NULL) {
    free(process);
    return NULL;
  }
  return process;
}

void cpw_process_init(cpwprocesss *process) {
  process->pid = 0;
  process->source = NULL;
  process->output = NULL;
  return;
}

int cpw_process_add_source(cpwsource *source, cpwprocess *process) {
  if (source) {
    if (process) {
      DL_APPEND(process->source, source);
      return true;
    }
  }
  return false;
}

int cpw_process_add_output(cpwoutput *output, cpwprocess *process) {
  if (output) {
    if (process) {
      DL_APPEND(process->output, output);
      return true;
    }
  }
  return false;
}

void cpw_process_free(cpwprocess *process) {
  cpwprocess *eprocess, *tprocess;
  if (process) {
    if (process->source) {
      DL_FOREACH_SAFE(process->source, eprocess, tprocess) {
	DL_DELETE(process->source, eprocess);
      }
    }
    if (process->output) {
      DL_FOREACH_SAFE(process->output, eprocess, tprocess) {
	DL_DELETE(process->output, eprocess);
      }
    }
    if (process->args)
      free(process->args);    
    free(process);
  }
}
