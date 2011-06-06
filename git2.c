#include <stdlib.h>
#include <stdio.h>

#include "common.h"

struct {
  char *name;
  git_cb fn;
} commands[] = {
  {"ls-remote", ls_remote},
  { NULL, NULL}
};

int run_command(git_cb fn, int argc, char **argv)
{
  int error;

  error = fn(NULL, argc, argv);
  if (error < GIT_SUCCESS)
    fprintf(stderr, "booh: %s\n", git_lasterror());

  return !!error;
}

int main(int argc, char **argv)
{
  int i, error;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <cmd> [repo]", argv[0]);
  }

  for (i = 0; commands[i].name != NULL; ++i) {
    if (!strcmp(argv[1], commands[i]))
      return run_command(commands[i].fn, --argc, ++argv);
  }

  fprintf(stderr, "Command not found: %s\n", argv[1]);
  
}
