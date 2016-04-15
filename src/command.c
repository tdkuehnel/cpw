#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "command.h"
#include "log.h"
#include "utlist.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


cpwcommand *cpw_command_new() {
  cpwcommand *command;
  command = calloc(1, sizeof(cpwcommand));
  if (command == NULL)
    return NULL;
  cpw_command_init(command);
  return command;
}

void cpw_command_init(cpwcommand *command) {
  int i;
  command->next = NULL;
  command->prev = NULL;
  return;
}

int cpw_command_add_path(cpwcommand *command, const char *path) {
  command->uname = path;
}

int cpw_command_set_value(cpwcommand *command, const char *cmd, const char *value) {
  if ( strcasecmp("path", cmd) == 0) {
    return cpw_command_add_path(command, value);
  } else {
    CPW_LOG_ERROR( "Invalid cmd for cpw_command_set_value: %s\n", cmd);
    return 1;
  }
}
