#include "common.h"
#include <git2.h>
#include <stdio.h>

static void show_refs(git_headarray *refs)
{
  int i;
  git_remote_head *head;

  if(refs->len == 0)
    puts("Everything up-to-date");

  for(i = 0; i < refs->len; ++i){
	  char oid[GIT_OID_HEXSZ + 1] = {0};
	  char *havewant;
	  head = refs->heads[i];
	  havewant = head->type == GIT_WHN_WANT ? "want" : "have";
	  git_oid_fmt(oid, &head->oid);
	  printf("%s\t%s\t%s\n", havewant, oid, head->name);
  }
}

int fetch(git_repository *repo, int argc, char **argv)
{
  git_remote *remote;
  git_headarray refs;
  git_config *cfg;
  int error;

  error = git_repository_config(&cfg, repo, NULL, NULL);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_get(&remote, cfg, argv[1]);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_connect(remote, GIT_DIR_FETCH);
  if (error < GIT_SUCCESS)
    return error;

  error = git_fetch_list_want(&refs, repo, remote);
  if (error < GIT_SUCCESS)
    return error;

  show_refs(&refs);

  error = git_fetch_negotiate(&refs, repo, remote);
  if (error < GIT_SUCCESS)
    return error;

  free(refs.heads);
  git_remote_free(remote);
  git_config_free(cfg);

  return GIT_SUCCESS;
}
