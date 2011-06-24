#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

static void show_refs(git_headarray *refs)
{
  int i;
  git_remote_head *head;

  for(i = 0; i < refs->len; ++i){
	  char oid[GIT_OID_HEXSZ + 1] = {0};
	  head = refs->heads[i];
	  git_oid_fmt(oid, &head->oid);
	  printf("%s\t%s\n", oid, head->name);
  }
}

int use_transport(char *url)
{
  int error;
  git_transport *t;
  git_headarray refs;

  error = git_transport_new(&t, url);
  if (error < GIT_SUCCESS) {
    fprintf(stderr, "Failed to create transport: %s", git_lasterror());
    return error;
  }

  error = git_transport_connect(t, GIT_DIR_FETCH);
  if (error < GIT_SUCCESS) {
    goto cleanup;
  }

  error = git_transport_ls(t, &refs);
  if (error < GIT_SUCCESS) {
    git__rethrow(error, "Failed to list refs");
    goto cleanup;
  }

  show_refs(&refs);

 cleanup:
  git_transport_close(t);
  git_transport_free(t);

  return error;
}

int use_remote(git_repository *repo, char *name)
{
  git_remote *remote = NULL;
  git_config *cfg = NULL;
  git_headarray refs;
  int error;

  error = git_repository_config(&cfg, repo, NULL, NULL);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_get(&remote, cfg, name);
  if (error < GIT_SUCCESS)
    goto cleanup;

  error = git_remote_connect(remote, GIT_DIR_FETCH);
  if (error < GIT_SUCCESS)
    goto cleanup;

  error = git_remote_ls(remote, &refs);
  if (error < GIT_SUCCESS)
    goto cleanup;

  show_refs(&refs);

  cleanup:
  git_config_free(cfg);
  git_remote_free(remote);

  return error;
}

int ls_remote(git_repository *repo, int argc, char **argv)
{
  git_headarray heads;
  git_remote_head *head;
  int error, i;

  /* If there's a ':' in the name, assume it's an URL */
  if (strchr(argv[1], ':') != NULL) {
    error = use_transport(argv[1]);
  } else {
    error = use_remote(repo, argv[1]);
  }

  return error;
}
