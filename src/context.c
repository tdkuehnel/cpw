
#include "context.h"

cpwcontext *cpw_context_new() {
  cpwcontext *context;
  context = calloc(1, sizeof(cpwcontext));
  return context;
}

void cpw_context_init(cpwcontext *context) {
  if (context) {
    context->config = calloc(1, sizeof(cpwconfig));
    context->arguments = calloc(1, sizeof(cpwarguments));    
  } else {
    CPW_LOG_WARNING("Invalid context passed to cpw_context_init\n");
  }
}

void cpw_context_free(cpwcontext *context) {
  if (context) {
    if (context->config) free(context->config);
    if (context->arguments) free(context->arguments);
  }
  free(context);
}

