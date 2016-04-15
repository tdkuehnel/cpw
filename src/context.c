
#include "context.h"

cpwcontext *cpw_context_new() {
  cpwcontext *context;
  context = calloc(1, sizeof(cpwcontext));
  return context;
}

void cpw_context_init(cpwcontext *context) {
}

void cpw_context_free(cpwcontext *context) {
  if (context) free(context);
}

