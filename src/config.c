#include <stdio.h>
#include <errno.h>
#include <string.h>

#define DEBUG 1
#define DEBUG_DEEP 0
#include "debug.h"

#include "log.h"
#include "context.h"
#include "config.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

const char* tags[] = {
  "Command",
  "Stream",
  "Job",
  "Process",
};

int cpw_config_init(cpwcontext *context) {
  return 1;
}
