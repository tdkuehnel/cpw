#include <stdio.h>
#include <errno.h>

#include "log.h"
#include "cpwconfig.h"

int cpw_config_parse(const char *filename, cpwconfig *config) {
    FILE *f;
    int ret = 0;
    int err;
    f = fopen(filename, "r");
    if (!f) {
      ret = errno;
      cpw_log(CPW_LOG_ERROR,
	      "Could not open the configuration file '%s'\n", filename);
      return ret;
    }



}
