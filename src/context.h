#ifndef CPW_CONTEXT_H
#define CPW_CONTEXT_H

#include <stdio.h>

struct cpwcontext;

#include "command.h"
#include "process.h"
#include "pipe.h"
#include "arg.h"
#include "log.h"
#include "config.h"

typedef struct cpwcontext {

  cpwconfig *config;
  cpwarguments *arguments;

  /* global main stuff */ 

  struct cpwpipe *globalinputlist;
  struct cpwpipe *globaloutputlist;

}cpwcontext;

cpwcontext *cpw_context_new();
void cpw_context_init(cpwcontext *context);
void cpw_context_free(cpwcontext *context);

#endif
