#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

int cpw_log_level = CPW_LOG_LEVEL_DEFAULT;

cpw_log_pointer log_ptr = NULL;

void cpw_log_printf( int level, const char *fmt, ... ) {
  if ( level >= cpw_log_level ) {
    va_list vl;
    va_start(vl, fmt);
    vprintf(fmt, vl);
    va_end(vl);
  }
}

void cpw_log_init() {
  log_ptr = &cpw_log_printf;
}

/*void cpw_log( int level, const char *fmt, ... ) {
  if (log_ptr != NULL ) {
    printf("Hello\n");
    (*log_ptr)(CPW_LOG_INFO, "Info log %d\n" , 1);
    
  }
}
*/
void cpw_log_done() {
}
