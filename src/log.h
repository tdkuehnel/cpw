#define CPW_LOG_QUIET   -8
#define CPW_LOG_FATAL   8
#define CPW_LOG_ERROR   16
#define CPW_LOG_WARNING 24
#define CPW_LOG_INFO    32
#define CPW_LOG_VERBOSE 40
#define CPW_LOG_DEBUG   48

#define CPW_LOG_LEVEL_DEFAULT 32

#define CPW_LOG(level, fmt, ...)					\
  if (log_ptr != NULL) { (*log_ptr)(level, fmt,  ## __VA_ARGS__); }

/* Function pointer to log function */

typedef void (*cpw_log_pointer)(int, const char *, ...);

extern cpw_log_pointer log_ptr;

void cpw_log_init();
void cpw_log_done();

/* Our different log functions */

void cpw_log_printf( int level, const char *fmt, ... );


