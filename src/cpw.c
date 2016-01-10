#include "config.h"
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "utlist.h"

#include "process.h"
#include "pipe.h"

#include "debug.h"

#define DEBUG 0

cpwpipe *globalinputlist;
cpwpipe *globaloutputlist;

int count = 0;
int last_signum = 0;
int r;
char *buf;
int gpid;

int teststream_start(cpwpipe *pipe) {
  char *args[CPW_PROCESS_MAX_ARGS];
  char arg[CPW_PROCESS_ARG_LEN + 1];
  int i, r;

  args[0] = strndup("ffmpeg", CPW_PROCESS_ARG_LEN - 1 );
  args[1] = strdup("-y");
  args[2] = strdup("-f");
  args[3] = strdup("lavfi");
  args[4] = strdup("-i");
  args[5] = strndup("testsrc=:size=320x240", CPW_PROCESS_ARG_LEN - 1 );
  args[6] = strdup("-f");
  args[7] = strdup("rawvideo");
  args[8] = strdup("-pix_fmt");
  args[9] = strdup("rgb24");
  args[10] = strdup("-video_size");
  args[11] = strdup("320x240");
  args[12] = pipe->name;
  args[13] = NULL;
  printf("ffmpeg args: %s %s %s %s %s %s %s %s %s %s %s %s %s\n", \
	 args[0], args[1], args[2], args[3], args[4], args[5],		\
	 args[6], args[7], args[8], args[9], args[10], args[11], args[12]);

  r = cpw_process_create("/home/tdk/bin/ffmpeg", args);
  if (r > 0) {
    printf("process created, id: %d\n", r);
    gpid = r;
    return 0;
  } else {
    printf("create process failed\n");
    return -1;
  }
}

void sig_handler(int signum) {
  count++;
  last_signum = signum;
}

int main(int argc, char **argv)
{
  int r, highestfd;
  fd_set rfds, wfds;
  sigset_t sigmask;

  struct sigaction new_action, old_action;
  cpwpipe *tpipe, *ipipe, *inpipe, *outpipe;

  new_action.sa_handler = sig_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction (SIGINT, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGINT, &new_action, NULL);
  sigaction (SIGHUP, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGHUP, &new_action, NULL);
  sigaction (SIGTERM, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGTERM, &new_action, NULL);

  /* main initialization ends here */

  /* initialize global pipelists */
  cpw_pipe_init();

  /* create test pipeline */
  inpipe = cpw_pipe_create_with_buflist("input01", PIPE_INPUT);
  if (inpipe == NULL) {
    printf("error creating inpipe test struct\n");
    exit(1);
  }

  outpipe = cpw_pipe_create("output01", PIPE_OUTPUT);
  if (outpipe == NULL) {
    printf("error creating outpipe test struct\n");
    cpw_pipe_free(inpipe);
    exit(1);
  }

  cpw_pipe_set_buflist(outpipe, inpipe->buflist);

  teststream_start(inpipe);

  cpw_pipe_register(inpipe);
  cpw_pipe_register(outpipe);

  /*  inpipe = cpw_pipe_create_with_buflist("input02", PIPE_INPUT);
  if (inpipe == NULL) {
    printf("error creating inpipe test struct\n");
    exit(1);
  }
  cpw_pipe_register(inpipe);
  */
  /* testpipeline initialization ends here */
  
  /* initialize main loop */
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  debug_printf("entering main loop\n");
 
  while(1) {
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    debug_printf("\n");
    /* watch for registered pipes */
    highestfd = 0;
    for(ipipe=globalinputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
      if ( ipipe->buflist == NULL ) break;
      if ( ipipe->buflist->empty != NULL ) {
	debug_printf("adding pipe to rfd set\n");
	FD_SET(ipipe->fd, &rfds);
	if ( ipipe->fd > highestfd ) highestfd = ipipe->fd; 
      } else {
	/*FD_CLR(ipipe->fd, &rfds);*/
      }
    }    
    for(ipipe=globaloutputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
      if ( ipipe->buflist == NULL ) break;
      if ( ipipe->buflist->inuse != NULL ) {
	debug_printf("adding pipe to wfd set\n");
	FD_SET(ipipe->fd, &wfds);
	if ( ipipe->fd > highestfd ) highestfd = ipipe->fd; 
      } else {
	/*	FD_CLR(ipipe->fd, &wfds);*/
	debug_printf("warning: buffer of outpipe %s empty (no bytes to write)\n", ipipe->name);
      }
    }

    /* need to get highest FD number to pass to pselect next */
    r = pselect(highestfd + 1, &rfds, &wfds, NULL, NULL, NULL);

    if (r > 0) { 
      debug_printf("input/ouput ready\n");
      /* first we check output channels */
      for(ipipe=globaloutputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
	if ( FD_ISSET(ipipe->fd, &wfds) ) { 
	  /* we need to write */
	  /*printf("."); fflush(stdout);*/
	  cpw_pipe_write(ipipe);
	}
      }
      
      for(ipipe=globalinputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
	/* input lags behind, partial read only, todo */
	if ( FD_ISSET(ipipe->fd, &rfds) ) { 
	  cpw_pipe_read(ipipe);
	}
      }
    } else {
      switch (errno) {
      case EBADF: 
	printf("error during pselect: EBADF\n");
	break;
	case EINTR: 
	  printf("signal during pselect: EINTR\n");
	  break;
      case EINVAL: 
	printf("error during pselect: EINVAL\n");
	break;
      case ENOMEM: 
	printf("error during pselect: ENOMEM\n");
	break;
      default:
	printf("error during pselect: unknown (should never happen)\n");
      }
    }
    if (last_signum != 0) {
      break;
    }
    /*sleep(1);*/
  }
  
  HASH_ITER(hh, globalinputlist, ipipe, tpipe) {
    cpw_pipe_free(ipipe);
  }    
  HASH_ITER(hh, globaloutputlist, ipipe, tpipe) {
    cpw_pipe_free(ipipe);
  }    
  printf("%d signals handled\n", count);
  kill(gpid, SIGTERM);
  printf("done\n");
  
  return 0;
  
}
