#include "context.h"

cpwcontext *cpw_context_new() {
  cpwcontext *context;
  context = calloc(1, sizeof(cpwcontext));
  return context;
}

void cpw_context_init(cpwcontext *context) {
  if (context) {
    context->config = cpw_config_new();
    context->arguments = calloc(1, sizeof(cpwarguments));    
  } else {
    CPW_LOG_WARNING("Invalid context passed to cpw_context_init\n");
  }
}

void cpw_context_free(cpwcontext **pcontext) {
  cpwcontext *context;
  if (pcontext) {
    context = *pcontext;
    if (context) {
      context = *pcontext;
      if (context->config) free(context->config);
      if (context->arguments) free(context->arguments);
      free(context);
      *pcontext = NULL;
    } else
      CPW_LOG_ERROR("Invalid context\n");
  } else 
    CPW_LOG_ERROR("Invalid argument (*context)\n");
}

