#define CPW_LOG_QUIET   -8
#define CPW_LOG_FATAL   8
#define CPW_LOG_ERROR   16
#define CPW_LOG_WARNING 24
#define CPW_LOG_INFO    32
#define CPW_LOG_VERBOSE 40
#define CPW_LOG_DEBUG   48

void cpw_log( int level, const char *fmt, ... );
