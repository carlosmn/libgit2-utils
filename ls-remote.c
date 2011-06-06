#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

int ls_remote(git_repository *repo, int argc, char **argv)
{
  git_transport *t;
  git_headarray heads;
  int error;

  error = git_transport_new(&t, repo, argv[1]);
  if (error < GIT_SUCCESS) {
    fprintf(stderr, "Failed to create transport: %s", git_lasterror());
    return error;
  }

  error = git_transport_connect(t, INTENT_PULL);
  if (error < GIT_SUCCESS) {
    fprintf(stderr, "Failed to connect transport: %s", git_lasterror());
    return error;
  }

  error = git_transport_ls(t, &heads);
  if (error < GIT_SUCCESS) {
    fprintf(stderr, "Failed to list refs: %s", git_lasterror());
    return error;
  }

  return GIT_SUCCESS;
}
