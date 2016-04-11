#ifndef CPW_COMMAND_H
#define CPW_COMMAND_H

#define CPW_COMMAND_MAX_LENGTH 256

typedef struct cpwcommand {
  struct cpwcommand *next, *prev;
  char *uname[CPW_COMMAND_MAX_LENGTH];
}cpwcommand;

#endif
