#ifndef CPW_PROCESS_H
#define CPW_PROCESS_H

#define CPW_PROCESS_MAX_ARGS 32
#define CPW_PROCESS_ARG_LEN 256

#include "pipe.h"

typedef enum source_type {
  CPW_SOURCE_TYPE_FALLBACK,
  CPW_SOURCE_TYPE_DROPIN
}source_type;

typedef struct cpwsource {
  struct cpwsource *next, *prev;
  source_type type;
  cpwpipe *pipe;
  cpwpipe *fallback;
}cpwsource;

typedef enum output_type {
  CPW_OUTPUT_TYPE_FALLBACK,
  CPW_OUTPUT_TYPE_DROPIN
}output_type;

typedef struct cpwoutput {
  struct cpwoutput *next, *prev;
  output_type type;
  cpwpipe *pipe;
}cpwoutput;

typedef struct cpwjob {
  int duration;
  char *args[CPW_PROCESS_MAX_ARGS];
}cpwjob;


typedef struct cpwprocess {
  struct cpwprocess *next, *prev;
  cpwcommand *command;
  const char *args[CPW_PROCESS_MAX_ARGS];
  char arg[CPW_PROCESS_ARG_LEN + 1];
  int pid;
  cpwsource *source;
  cpwoutput *output;
}cpwprocess;

cpwprocess *cpw_process_new();
void cpw_process_init(cpwprocess *process);
int cpw_process_add_source(cpwprocess *process, cpwsource *source);
int cpw_process_add_output(cpwprocess *process, cpwoutput *output);
int cpw_process_add_arg(cpwprocess *process, const char *arg);

int cpw_process_set_value(cpwprocess *process, const char *cmd, const char *value);

void cpw_process_free(cpwprocess *process);

/* process related things may go in a seperate file */
int cpw_process_create(char *execpath, char *args[CPW_PROCESS_MAX_ARGS]);

#endif
