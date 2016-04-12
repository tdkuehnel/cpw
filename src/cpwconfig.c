#include <stdio.h>
#include <errno.h>

#define DEBUG 0
#include "debug.h"

#include "log.h"
#include "cpwconfig.h"

int cpw_config_parse(const char *filename, cpwconfig *config) {
    FILE *f;
    int ret = 0;
    int err;
    f = fopen(filename, "r");
    if (!f) {
      ret = errno;
      CPW_DEBUG( "Could not open the configuration file '%s'\n", filename);
      return ret;
    }



}
