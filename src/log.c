#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

#define DEBUG 0
#include "debug.h"

int cpw_log_level = CPW_LOG_LEVEL_DEFAULT;

cpw_log_pointer log_ptr = NULL;

void cpw_log_printf( int level, const char *fmt, ... ) {
  CPW_DEBUG("called log with level: %d, current log_level: %d\n", level, cpw_log_level);
  if ( level <= cpw_log_level ) {
    va_list vl;
    va_start(vl, fmt);
    vprintf(fmt, vl);
    va_end(vl);
  }
}

void cpw_log_init(int level) {
  log_ptr = &cpw_log_printf;
  cpw_log_level = level * 8;
  
  CPW_DEBUG("set log level to: %d\n", cpw_log_level);
}

void cpw_log_done() {
}
