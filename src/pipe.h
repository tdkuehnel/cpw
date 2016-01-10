#ifndef CPW_PIPE_H
#define CPW_PIPE_H

#define PIPE_DIR "/tmp/"
#define NAME_BUF_LEN   256
#define INPUT_PIPE_NUM 32
#define OUTPUT_PIPE_NUM 32
#define FRAME_SIZE 230400
#define MAX_FRAME_SIZE 1024 * 1024
#define PIPE_BUF_DEPTH 2
#define MAX_PIPE_BUF_DEPTH 32

#include "uthash.h"

typedef struct cpwpipebuf {
  struct cpwpipebuf *next;
  struct cpwpipebuf *prev;
  int pos;
  int size;
  void *buf;
}cpwpipebuf;

typedef struct cpwbuflist {
  cpwpipebuf *empty;
  cpwpipebuf *inuse;
  int numbuf;
}cpwbuflist;

typedef enum pipe_type {
  PIPE_INPUT,
  PIPE_OUTPUT
}pipe_type;

typedef enum pipe_status {
  CPW_PIPE_STATUS_OPEN,
  CPW_PIPE_STATUS_CLOSED,
}pipe_status;

typedef struct cpwpipe {
  struct cpwpipe *next;
  struct cpwpipe *prev;
  UT_hash_handle hh;  
  pipe_type type;
  pipe_status status;
  int fd;
  char *name;
  cpwbuflist *buflist;
}cpwpipe;

extern cpwpipe *globalinputlist;
extern cpwpipe *globaloutputlist;

void cpw_pipe_init();
cpwpipe *cpw_pipe_create_with_buflist(char *name, pipe_type type);
cpwpipe *cpw_pipe_create(char *name, pipe_type type);
void cpw_pipe_read(cpwpipe *pipe);
void cpw_pipe_write(cpwpipe *pipe);
int cpw_pipe_register(cpwpipe *pipe);
int cpw_pipe_sort(cpwpipe *a, cpwpipe *b);
void cpw_pipe_free(cpwpipe *pipe);
cpwbuflist *cpw_pipe_set_buflist(cpwpipe *pipe, cpwbuflist *buflist);

cpwbuflist *cpw_buflist_new(int depth);
void cpw_buflist_init(cpwbuflist *buflist);
void cpw_buflist_buffer_full(cpwbuflist *buflist, cpwpipebuf *pipebuf);
void cpw_buflist_buffer_empty(cpwbuflist *buflist, cpwpipebuf *pipebuf);
void cpw_buflist_free(cpwbuflist *buflist);

cpwpipebuf *cpw_pipebuf_new(int bufsize); 
int cpw_pipebuf_init(cpwpipebuf *pipebuf, int bufsize);
void cpw_pipebuf_free(cpwpipebuf *pipebuf);


#endif
