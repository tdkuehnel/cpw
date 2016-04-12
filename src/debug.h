#ifndef CPW_DEBUG_H
#define CPW_DEBUG_H

/* simple debug implementation which gets compiled but optimized out if DEBUG not 1
   see http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
   
   ALLWAYS put a #define DEBUG 0 (or DEBUG_DEEP 0) in front of your #include "debug.h" 
   when using the macros or the compiler will complain.

   gcc -DDEBUG ( or -DDEBUG_DEEP ) will be overwritten. 
*/

#define CPW_DEBUG(...) \
  do { if (DEBUG) fprintf(stderr, ## __VA_ARGS__); } while (0)

#define CPW_DEBUG_DEEP(...) \
  do { if (DEBUG_DEEP) fprintf(stderr, ## __VA_ARGS__); } while (0)


#endif
