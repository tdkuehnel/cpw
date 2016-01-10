#ifndef CPW_DEBUG_H
#define CPW_DEBUG_H
#define debug_printf(...) \
  do { if (DEBUG) fprintf(stderr, ## __VA_ARGS__); } while (0)

#define debug2_printf(...) \
  do { if (DEBUG2) fprintf(stderr, ## __VA_ARGS__); } while (0)
#endif
