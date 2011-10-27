#include <stdlib.h>
#include <stdio.h>

#include "common.h"

struct {
  char *name;
  git_cb fn;
} commands[] = {
  {"ls-remote", ls_remote},
  // {"parse-pkt-line", parse_pkt_line},
  //  {"show-remote", show_remote},
  {"fetch", fetch},
  {"index-pack", index_pack},
  {"status", status},
  { NULL, NULL}
};

int run_command(git_cb fn, int argc, char **argv)
{
  int error;
  git_repository *repo;

  error = git_repository_open(&repo, ".git");
  if (error < GIT_SUCCESS)
    repo = NULL;

  error = fn(repo, argc, argv);
  if (error < GIT_SUCCESS)
    fprintf(stderr, "booh:\n %s\n", git_lasterror());

  if(repo)
    git_repository_free(repo);

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
