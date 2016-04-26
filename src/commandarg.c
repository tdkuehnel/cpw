#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "commandarg.h"
#include "log.h"
#include "utlist.h"

#include "debug.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#undef DEBUG
#define DEBUG 0

cpwcommandargindex *cpw_commandargindex_new(int index) {
  cpwcommandargindex *commandargindex;

  commandargindex = calloc(1, sizeof(cpwcommandargindex));
  if (commandargindex == NULL)
    return NULL;
  commandargindex->index = index;
  return commandargindex;
}

cpwcommandarg *cpw_commandarg_new() {
  cpwcommandarg *commandarg;

  commandarg = calloc(1, sizeof(cpwcommandarg));
  if (commandarg == NULL)
    return NULL;
  cpw_commandarg_init(commandarg);
  return commandarg;
}

void cpw_commandarg_init(cpwcommandarg *commandarg) {
  commandarg->next = NULL;
  commandarg->prev = NULL;
  commandarg->input = NULL;
  commandarg->output = NULL;
  return;
}

void cpw_commandarg_done(cpwcommandarg **pcommandarg) {
  cpwcommandarg *commandarg;
  cpwcommandargindex *element, *tmp;

  if ( pcommandarg ) {
    commandarg = *pcommandarg;
    if ( commandarg ) {
      LL_FOREACH_SAFE(commandarg->input, element , tmp) {
	LL_DELETE(commandarg->input, element);
	free( element );
      }    
      LL_FOREACH_SAFE(commandarg->output, element , tmp) {
	LL_DELETE(commandarg->output, element);
	free( element );
      }    
      *pcommandarg = NULL;
    } else {
      CPW_LOG_ERROR("Invalid commandarg\n");
    }
  } else {
    CPW_LOG_ERROR("Invalid argument (*pcommandarg)\n");
  }  
}

