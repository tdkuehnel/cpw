#ifndef CPW_PROCESS_H
#define CPW_PROCESS_H

#define CPW_PROCESS_MAX_ARGS 32
#define CPW_PROCESS_ARG_LEN 256

typedef struct cpwjob {
  int duration;
  char *args[FFMPEG_MAX_ARGS];
  cpwpipe **pipes[];
}cpwjob;

typedef struct cpwprocess {
  char *args[CPW_PROCESS_MAX_ARGS];
  char arg[CPW_PROCESS_ARG_ARG_LEN + 1];
  int pid;
  cpwpipe *source;
  cpwpipe *output;
}cpwprocess;

cpwprocess *cpw_process_new();
void cpw_process_init(cpwprocesss *process);
int cpw_process_add_source(cpwsource *source, cpwprocess *process);
int cpw_process_add_output(cpwoutput *output, cpwprocess *process);
void cpw_process_free(cpwprocess *process);

#endif
