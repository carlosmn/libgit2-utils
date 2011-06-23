#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

int ls_remote(git_repository *repo, int argc, char **argv)
{
  git_transport *t = NULL;
  git_headarray heads;
  git_remote_head *head;
  int error, i;

  error = git_transport_new(&t, argv[1]);
  if (error < GIT_SUCCESS) {
    fprintf(stderr, "Failed to create transport: %s", git_lasterror());
    return error;
  }

  error = git_transport_connect(t, INTENT_PULL);
  if (error < GIT_SUCCESS) {
    goto cleanup;
  }

  error = git_transport_ls(t, &heads);
  if (error < GIT_SUCCESS) {
    git__rethrow(error, "Failed to list refs");
    goto cleanup;
  }

  for(i = 0; i < heads.len; ++i){
	  char oid[GIT_OID_HEXSZ + 1] = {0};
	  head = heads.heads[i];
	  git_oid_fmt(oid, &head->oid);
	  printf("%s\t%s\n", oid, head->name);
  }

 cleanup:
  git_transport_close(t);
  git_transport_free(t);

  return GIT_SUCCESS;
}
