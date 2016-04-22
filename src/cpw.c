#include "autoconfig.h"
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

#include "context.h"

#define DEBUG 0
#include "debug.h"

cpwcontext *context;

int count = 0;
int last_signum = 0;
int r;
char *buf;
int gpid;

void sig_handler(int signum) {
  count++;
  last_signum = signum;
}

struct sigaction new_action, old_action;
cpwpipe *tpipe, *ipipe, *inpipe, *outpipe;

void cpw_init(int argc, char **argv) {
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

  /* command line parsing */
  /* initialize global pipelists */
  context = cpw_context_new();
  cpw_context_init(context);
  
  cpw_arg_parse(context->arguments, argc, argv);
  cpw_log_init(context->arguments->log_level);
  CPW_LOG_INFO("%s\n", PACKAGE_STRING);

  /* read in config */
  cpw_config_init(context->config, context->arguments->config_file);
  cpw_config_validate(context->config);
  cpw_config_parse(context->config);

  /* initialize global pipelists */
  cpw_pipe_init(context);

  /* main initialization ends here */
}

int main(int argc, char **argv)
{
  int r, highestfd;
  fd_set rfds, wfds;
  sigset_t sigmask;

  cpw_init(argc, argv);

  /* create test pipeline */
  inpipe = cpw_pipe_create_with_buflist("input01", PIPE_INPUT);
  if (inpipe == NULL) {
    CPW_LOG(CPW_LOG_LEVEL_ERROR, "error creating inpipe test struct\n");
    cpw_log_done();
    exit(1);
  }

  outpipe = cpw_pipe_create("output01", PIPE_OUTPUT);
  if (outpipe == NULL) {
    CPW_LOG(CPW_LOG_LEVEL_ERROR, "error creating outpipe test struct\n");
    cpw_pipe_free(inpipe);
    cpw_log_done();
    exit(1);
  }

  cpw_pipe_set_buflist(outpipe, inpipe->buflist);

  /*teststream_start(inpipe);*/

  cpw_pipe_register(context, inpipe);
  cpw_pipe_register(context, outpipe);

  /*  inpipe = cpw_pipe_create_with_buflist("input02", PIPE_INPUT);
  if (inpipe == NULL) {
    CPW_LOG(CPW_LOG_ERROR, "error creating inpipe test struct\n");
    exit(1);
  }
  cpw_pipe_register(inpipe);
  */
  /* testpipeline initialization ends here */
  
  /* initialize main loop */
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  CPW_LOG( CPW_LOG_LEVEL_INFO, "entering main loop, CTRL-C to bail out\n");
 
  while(1) {
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    CPW_DEBUG("\n");
    /* watch for registered pipes */
    highestfd = 0;
    for(ipipe=context->globalinputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
      if ( ipipe->buflist == NULL ) break;
      if ( ipipe->buflist->empty != NULL ) {
	CPW_DEBUG("adding pipe to rfd set\n");
	FD_SET(ipipe->fd, &rfds);
	if ( ipipe->fd > highestfd ) highestfd = ipipe->fd; 
      } else {
	/*FD_CLR(ipipe->fd, &rfds);*/
      }
    }    
    for(ipipe=context->globaloutputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
      if ( ipipe->buflist == NULL ) break;
      if ( ipipe->buflist->inuse != NULL ) {
	CPW_DEBUG("adding pipe to wfd set\n");
	FD_SET(ipipe->fd, &wfds);
	if ( ipipe->fd > highestfd ) highestfd = ipipe->fd; 
      } else {
	/*	FD_CLR(ipipe->fd, &wfds);*/
	CPW_DEBUG("warning: buffer of outpipe %s empty (no bytes to write)\n", ipipe->name);
      }
    }

    /* need to get highest FD number to pass to pselect next */
    r = pselect(highestfd + 1, &rfds, &wfds, NULL, NULL, NULL);

    if (r > 0) { 
      CPW_DEBUG("input/ouput ready\n");
      /* first we check output channels */
      for(ipipe=context->globaloutputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
	if ( FD_ISSET(ipipe->fd, &wfds) ) { 
	  /* we need to write */
	  /*printf("."); fflush(stdout);*/
	  cpw_pipe_write(ipipe);
	}
      }
      
      for(ipipe=context->globalinputlist; ipipe != NULL; ipipe=ipipe->hh.next) {      
	/* input lags behind, partial read only, todo */
	if ( FD_ISSET(ipipe->fd, &rfds) ) { 
	  cpw_pipe_read(ipipe);
	}
      }
    } else {
      switch (errno) {
      case EBADF: 
	CPW_LOG(CPW_LOG_LEVEL_ERROR, "error during pselect: EBADF\n");
	break;
      case EINTR: 
	CPW_LOG(CPW_LOG_LEVEL_INFO, " signal during pselect: EINTR\n");
	break;
      case EINVAL: 
	CPW_LOG(CPW_LOG_LEVEL_ERROR, "error during pselect: EINVAL\n");
	break;
      case ENOMEM: 
	CPW_LOG(CPW_LOG_LEVEL_ERROR, "error during pselect: ENOMEM\n");
	break;
      default:
	CPW_LOG(CPW_LOG_LEVEL_ERROR, "error during pselect: unknown (should never happen)\n");
      }
    }
    if (last_signum != 0) {
      break;
    }
    /*sleep(1);*/
  }
  
  HASH_ITER(hh, context->globalinputlist, ipipe, tpipe) {
    cpw_pipe_free(ipipe);
  }    
  HASH_ITER(hh, context->globaloutputlist, ipipe, tpipe) {
    cpw_pipe_free(ipipe);
  }    
  CPW_LOG(CPW_LOG_LEVEL_INFO, "%d signals handled\n", count);
  kill(gpid, SIGTERM);

  cpw_context_free(&context);
  CPW_LOG(CPW_LOG_LEVEL_INFO, "done\n");
  cpw_log_done();
  
  return 0;
  
}
