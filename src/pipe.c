#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "utlist.h"

#include "pipe.h"

#include "debug.h"

#define DEBUG 0
#define DEBUG2 0

void cpw_pipe_init(){
  globalinputlist = NULL;
  globaloutputlist = NULL;
}

int cpw_pipe_register(cpwpipe *pipe) {
  int i;
  cpwpipe *p;
  if ( pipe != NULL ) { 
    if ( pipe->type == PIPE_INPUT ) {
      HASH_ADD_INT(globalinputlist, fd, pipe);      
    }
    if ( pipe->type == PIPE_OUTPUT ) {
      HASH_ADD_INT(globaloutputlist, fd, pipe);      
    }
  } else {
    debug_printf("no pipe to register\n");
    return 0;
  }
}

int cpw_pipe_sort(cpwpipe *a, cpwpipe *b) {  
  return (a->fd - b->fd);
}

void cpw_pipe_read(cpwpipe *pipe) {
  int readbytes;
  void *pbuf;
  int r;
  if ( pipe ) {
    if ( pipe->buflist ) {
      if ( pipe->buflist->empty ) {
	if ( pipe->buflist->empty->buf ) {
	  if ( pipe->buflist->empty->pos  < pipe->buflist->empty->size ) {
	    debug2_printf("\nreading from pipe %s, buffer pos %d\n", pipe->name, pipe->buflist->empty->pos);
	    readbytes = pipe->buflist->empty->size - pipe->buflist->empty->pos;
	    debug2_printf("readbytes: 0x%08x, (%d)\n", readbytes, readbytes);
	    pbuf = pipe->buflist->empty->buf + pipe->buflist->empty->pos;
	    debug2_printf("buf: 0x%08x, pbuf: 0x%08x, difference: 0x%08x\n", pipe->buflist->empty->buf, pbuf, pbuf - pipe->buflist->empty->buf);

	    r = read( pipe->fd, pbuf, readbytes );
	    debug2_printf("read 0x%08x (%d) bytes, framesize 0x%08x (%d)\n", r, r, pipe->buflist->empty->size, pipe->buflist->empty->size);
	    if ( r >= 0 ) {
	      if ( r > 0 ) {
		if ( r == readbytes ) {
		  /* read complete buffer */
		  pipe->buflist->empty->pos += r;		  
		  debug2_printf("buffer pos now: 0x%08x, next read from other buffer\n", pipe->buflist->empty->pos);
		  cpw_buflist_buffer_full(pipe->buflist, pipe->buflist->empty);
		} else {
		  /* read part of buffer */
		  pipe->buflist->empty->pos += r;
		  debug2_printf("buffer pos now: 0x%08x, next read at 0x%08x\n", \
				pipe->buflist->empty->pos, pipe->buflist->empty->buf + pipe->buflist->empty->pos);
		}
	      } else {
		debug_printf("eof from pipe %s detected\n", pipe->name);		
		close(pipe->fd);
		pipe->fd = open((char*)pipe->name, O_RDONLY|O_NONBLOCK);
		if (pipe->fd < 0) {
		  debug2_printf("error opening pipe %s%s\n", PIPE_DIR, pipe->name);
		  pipe->status = CPW_PIPE_STATUS_CLOSED;
		} else {
		  debug2_printf("pipe %s%s reopened\n", PIPE_DIR, pipe->name);
		  pipe->status = CPW_PIPE_STATUS_OPEN;
		}
	      }
	    } else {
	      debug_printf("error during read %d bytes to buffer from pipe %s\n", readbytes, pipe->name);
	    }
	  } else {
	    debug_printf("read buffer in empty list but full\n");
	  } 
	}
      }      
    }
  }
}

void cpw_pipe_write(cpwpipe *pipe) {
  int writebytes;
  void *pbuf;
  int r;
  if ( pipe ) {
    if ( pipe->buflist ) {
      if ( pipe->buflist->inuse ) {
	if ( pipe->buflist->inuse->buf ) {
	  if ( pipe->buflist->inuse->pos < pipe->buflist->inuse->size ) {
	    debug_printf("writing to pipe %s, buffer pos %d\n", pipe->name, pipe->buflist->inuse->pos);
	    writebytes = pipe->buflist->inuse->size - pipe->buflist->inuse->pos;
	    pbuf = pipe->buflist->inuse->buf + pipe->buflist->inuse->pos;
	    r = write( pipe->fd, pbuf, writebytes );
	    debug_printf("%d bytes written, framesize %d\n", r, pipe->buflist->inuse->size);
	    if ( r >= 0 ) {
	      if ( r > 0 ) {
		if ( r == writebytes ) {
		  /* complete or rest of buffer written */
		  pipe->buflist->inuse->pos += r;		  
		  cpw_buflist_buffer_empty(pipe->buflist, pipe->buflist->inuse);
		} else {
		  /* read part of  buffer */
		  pipe->buflist->inuse->pos += r;
		}
	      } else {
		debug_printf("no bytes written to %s\n", pipe->name);		
	      }
	    } else {
	      debug_printf("error during write of %d bytes from buffer to pipe %s\n", writebytes, pipe->name);
	    }
	  } else {
	    debug_printf("write buffer in inuse list but empty\n");
	  } 
	}
      }      
    }
  }
}

cpwpipe *cpw_pipe_create_with_buflist(char *name, pipe_type type) {
  cpwpipe *pipe;
  pipe = cpw_pipe_create(name, type);
  if ( pipe == NULL ) {
    debug_printf("error creating cpwpipe\n");
    return NULL;
  }
  pipe->buflist = cpw_buflist_new(PIPE_BUF_DEPTH); 
  if (pipe->buflist == NULL) {
    debug_printf("error creating buflist\n");
    free(pipe->name);
    free(pipe);
    return NULL;
  }
  return pipe;
}

cpwpipe *cpw_pipe_create(char *name, pipe_type type) {
  cpwpipe *pipe;
  int r;
  pipe = calloc(1, sizeof(cpwpipe));
  if (pipe == NULL)
    return NULL;
  pipe->name = calloc(1, NAME_BUF_LEN);
  if (pipe->name == NULL) {
    free(pipe);
    return NULL;
  }
  pipe->type = type;
  pipe->next = NULL;
  pipe->prev = NULL;
  snprintf(pipe->name, NAME_BUF_LEN, "%s%s", PIPE_DIR, name);
  r = mkfifo((char*)pipe->name, 0666);
  if ( r != 0)
    debug_printf("error creating fifo %s%s, may exist\n", PIPE_DIR, name);
  else
    debug_printf("fifo %s%s created\n", PIPE_DIR, name);
  switch ( type ) {
  case PIPE_INPUT:
    pipe->fd = open((char*)pipe->name, O_RDONLY|O_NONBLOCK);
    break;
  case PIPE_OUTPUT:
    pipe->fd = open((char*)pipe->name, O_WRONLY|O_NONBLOCK);
    break;    
  }
  if (pipe->fd < 0) {
    debug_printf("error opening pipe %s%s\n", PIPE_DIR, name);
    pipe->status = CPW_PIPE_STATUS_CLOSED;
  } else {
    debug_printf("pipe %s%s opened\n", PIPE_DIR, name);
    pipe->status = CPW_PIPE_STATUS_OPEN;
  }
  /* cpw_pipe_register(pipe); */
  debug_printf("cpw pipe created\n");
  return pipe;
}

cpwbuflist *cpw_pipe_set_buflist(cpwpipe *pipe, cpwbuflist *buflist) {
  cpwbuflist *tlist;
  if ( pipe != NULL ) {
    if ( buflist != NULL ) {
      if ( pipe->buflist != NULL ) {
	tlist = pipe->buflist;
	pipe->buflist = buflist;
	return tlist;
      } else {
	pipe->buflist = buflist;
	return NULL;	
      }
    }
  }
}

void cpw_pipe_free(cpwpipe *pipe) {
  if ( pipe != NULL ) {
    debug_printf("freeing pipe resources\n");
    close(pipe->fd);
    cpw_buflist_free(pipe->buflist);
    if (pipe->name != NULL) free(pipe->name);
    free(pipe);
  }
}

cpwbuflist *cpw_buflist_new(int depth) {
  cpwbuflist *buflist;
  cpwpipebuf *pipebuf;
  int i;
  if ( depth <= 0 ) {
    printf("invalid pipebufdepth of %d, aborting\n", depth);
    return NULL;
  }
  if (depth > MAX_PIPE_BUF_DEPTH ) {
    depth = MAX_PIPE_BUF_DEPTH;
    printf("pipebufdepth %d  exceeds MAX_PIPE_BUF_DEPTH %d, clamping\n", depth, MAX_PIPE_BUF_DEPTH); 
  }
  buflist = malloc(sizeof(cpwbuflist));
  if ( buflist == NULL ) {
    printf("error getting mem for buflist\n");
    return NULL;
  } else {
    cpw_buflist_init(buflist);
    for ( i = 0; i < depth; i++ ) {
      pipebuf = cpw_pipebuf_new(FRAME_SIZE);
      if ( pipebuf == NULL ) {
	printf("error creating pipebuf, depth %d, aborting\n", i);
	cpw_buflist_free(buflist);
	return NULL;
      }
      DL_APPEND(buflist->empty, pipebuf);      
      debug_printf("pipebuf depth %d created and appended to list\n", i);
    }
    return buflist;    
  }
}

void cpw_buflist_init(cpwbuflist *buflist) {
  if ( buflist != NULL ) {
    buflist->empty = NULL;
    buflist->inuse = NULL;
  }
}

void cpw_buflist_buffer_full(cpwbuflist *buflist, cpwpipebuf *pipebuf) {
  cpwpipebuf *tbuf;
  if ( buflist ) {
    if ( pipebuf ) {
      if ( pipebuf->pos == pipebuf->size ) {
	tbuf = NULL;
	LL_SEARCH_SCALAR(buflist->empty, tbuf, buf, pipebuf->buf );
	if ( tbuf == pipebuf ) {
	  DL_DELETE(buflist->empty, pipebuf);
	  pipebuf->pos = 0;
	  DL_APPEND(buflist->inuse, pipebuf);
	} else {
	  printf("buffer not in list to switch to full\n");
	}
      } else {
	printf("pipebuf not full to switch to inuse list\n");
      }
    }
  }
}

void cpw_buflist_buffer_empty(cpwbuflist *buflist, cpwpipebuf *pipebuf) {
  cpwpipebuf *tbuf;
  if ( buflist ) {
    if ( pipebuf ) {
      if ( pipebuf->pos == pipebuf->size ) {
	tbuf = NULL;
	LL_SEARCH_SCALAR(buflist->inuse, tbuf, buf, pipebuf->buf );
	if ( tbuf == pipebuf ) {
	  DL_DELETE(buflist->inuse, pipebuf);
	  pipebuf->pos = 0;
	  DL_APPEND(buflist->empty, pipebuf);
	} else {
	  printf("buffer not in list to switch to empty\n");
	}
      } else {
	printf("pipebuf not empty to switch to empty list\n");
      }
    }
  }
}

void cpw_buflist_free(cpwbuflist *buflist) {
  cpwpipebuf *ibuf;
  cpwpipebuf *tbuf;  
  if ( buflist != NULL ) {
    DL_FOREACH_SAFE(buflist->empty,ibuf,tbuf) {
      DL_DELETE(buflist->empty, ibuf);
      cpw_pipebuf_free(ibuf);
    }
    DL_FOREACH_SAFE(buflist->inuse, ibuf, tbuf) {
      DL_DELETE(buflist->inuse, ibuf);
      cpw_pipebuf_free(ibuf);
    }
  } else {
    printf("no buflist to free\n");
  }
}

cpwpipebuf *cpw_pipebuf_new(int bufsize) {
  cpwpipebuf *pipebuf;
  pipebuf = malloc(sizeof(cpwpipebuf));
  if ( pipebuf == NULL ) {
    printf("error allocating mem for struct pipebuf\n");
    return  NULL;
  } else {
    cpw_pipebuf_init(pipebuf, bufsize);
    return pipebuf;
  }
}
 
int cpw_pipebuf_init(cpwpipebuf *pipebuf, int bufsize) {
  if ( pipebuf != NULL ) {
    if ( bufsize <= MAX_FRAME_SIZE ) {
      pipebuf->buf = malloc(bufsize);
      if ( pipebuf->buf != NULL ) {
	pipebuf->size = bufsize;
	pipebuf->pos = 0;
	pipebuf->next = NULL;
	pipebuf->prev = NULL;
	return 0;
      } else {
	printf("error init pipebuf: malloc of size %d failed\n", bufsize);
	return -1;	
      }
    } else {
      printf("error init pipebuf: framesize %d exceeds MAX_FRAME_SIZE %d\n", bufsize, MAX_FRAME_SIZE);
      return -1;
    }
  } else {
    printf("error init pipebuf: buf parameter invalid\n");
    return -1;
  }
}

void cpw_pipebuf_free(cpwpipebuf *pipebuf) {
  if ( pipebuf != NULL ) {
    if ( pipebuf->buf != NULL ) {
      free(pipebuf->buf);
    }
  }
}


