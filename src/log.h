#ifndef _CPW_LOG_
#define _CPW_LOG_

#define CPW_LOG_LEVEL_QUIET   0
#define CPW_LOG_LEVEL_FATAL   8
#define CPW_LOG_LEVEL_ERROR   16
#define CPW_LOG_LEVEL_WARNING 24
#define CPW_LOG_LEVEL_INFO    32
#define CPW_LOG_LEVEL_VERBOSE 40
#define CPW_LOG_LEVEL_DEBUG   48

#define CPW_LOG_LEVEL_DEFAULT 32

#define CPW_LOG(level, fmt, ...)					\
  if (log_ptr != NULL) { (*log_ptr)(level, fmt,  ## __VA_ARGS__); }

#define CPW_LOG_INFO(fmt, ...) CPW_LOG(CPW_LOG_LEVEL_INFO, fmt, ## __VA_ARGS__);
#define CPW_LOG_ERROR(fmt, ...) CPW_LOG(CPW_LOG_LEVEL_ERROR, fmt, ## __VA_ARGS__);
#define CPW_LOG_WARNING(fmt, ...) CPW_LOG(CPW_LOG_LEVEL_WARNING, fmt, ## __VA_ARGS__);

/* Function pointer to log function */

typedef void (*cpw_log_pointer)(int, const char *, ...);

extern cpw_log_pointer log_ptr;

void cpw_log_init(int level);
void cpw_log_done();

/* Our different log functions */

void cpw_log_printf( int level, const char *fmt, ... );

#endif
