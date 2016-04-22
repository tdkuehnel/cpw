#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "command.h"
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

cpwcommand *cpw_command_new() {
  cpwcommand *command;

  command = calloc(1, sizeof(cpwcommand));
  if (command == NULL)
    return NULL;
  cpw_command_init(command);
  return command;
}

void cpw_command_init(cpwcommand *command) {
  command->next = NULL;
  command->prev = NULL;
  command->args = NULL;
  return;
}

#undef DEBUG
#define DEBUG 0

int cpw_command_set_path(cpwcommand *command, const char *path) {
  CPW_DEBUG("cpwcommand: setting value 'path' to %s\n", path);
  strncpy(command->path, path, CPW_COMMAND_MAX_ARG_LENGTH);
  command->path[CPW_COMMAND_MAX_ARG_LENGTH - 1] = '\0';
  return 1;
}

int cpw_command_set_empty_path(cpwcommand *command, const char *path) {
  if ( *command->path == '\0' )
    return cpw_command_set_path(command, path);
  else
    CPW_LOG_ERROR("error setting empty path to %s, path already set to %s\n", path, command->path);
  return 0;
}

int cpw_command_set_name(cpwcommand *command, const char *name) {
  char uname[CPW_COMMAND_MAX_ARG_LENGTH];
  char *p;

  p = uname;
  while ( *name != '\0' && *name != '>' ) {
    *p++ = *name++;    
  }
  *p = '\0';

  CPW_DEBUG("cpwcommand: setting value 'name' to %s\n", uname);
  strncpy(command->name, uname, CPW_COMMAND_MAX_ARG_LENGTH);
}

int cpw_command_set_empty_name(cpwcommand *command, const char *name) {
  if ( *command->name == '\0' )
    return cpw_command_set_name(command, name);
  else
    CPW_LOG_ERROR("error setting empty name to %s, name already set to %s\n", name, command->name);
  return 0;
}

int cpw_command_add_arg(cpwcommand *command, const char *arg) {
  cpwcommandarg *newarg;
  newarg = cpw_commandarg_new();
  if ( newarg ) {
    strncpy(newarg->arg, arg, CPW_COMMAND_MAX_ARG_LENGTH - 1);
    newarg->arg[CPW_COMMAND_MAX_ARG_LENGTH - 1] = '\0';
    
    LL_APPEND(command->args, newarg);
    CPW_DEBUG("cpwcommand: adding value '%s' to arglist\n", arg);
    return 1;
  } else {
    CPW_LOG_ERROR("error getting mem for commandarg\n");
    return 0;
  }
}

int cpw_command_set_value(cpwcommand *command, const char *arg, const char *value) {
  if ( strcasecmp("path", arg) == 0) {
    return cpw_command_set_path(command, value);
  } else if ( strcasecmp("name", arg) == 0 ) {
    return cpw_command_set_name(command, value);
  } else if ( strcasecmp("arg", arg) == 0 ) {
    return cpw_command_add_arg(command, value);
  } else {
    CPW_LOG_ERROR( "Invalid arg for cpw_command_set_value: %s\n", arg);
    return 1;
  }
}

void cpw_command_done(cpwcommand **pcommand) {
  cpwcommand *command;
  cpwcommandarg *element, *tmp;

  if ( pcommand ) {
    command = *pcommand;
    if ( command ) {
      LL_FOREACH_SAFE(command->args, element , tmp) {
	LL_DELETE(command->args, element);
	free( element );
      }    
      *pcommand = NULL;
    } else {
      CPW_LOG_ERROR("Invalid command\n");
    }
  } else {
    CPW_LOG_ERROR("Invalid argument (*pcommand)\n");
  }  
}
