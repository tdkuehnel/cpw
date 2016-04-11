#include <stdarg.h>

#include "log.h"

int cpw_log_level = CPW_LOG_WARNING;

void cpw_log( int level, const char *fmt, ... ) {
  if ( level >= cpw_log_level ) {
    va_list vl;
    va_start(vl, fmt);
    vprintf(fmt, vl);
    va_end(vl);
  }
}
