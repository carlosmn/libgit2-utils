#include "common.h"
#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	  git_oid_fmt(oid, &head->oid);
	  printf("%s\t%s\n", oid, head->name);
  }
}

static int rename_packfile(char *packname, git_indexer *idx)
{
  char path[GIT_PATH_MAX], oid[GIT_OID_HEXSZ + 1], *slash;
  int ret;

  strcpy(path, packname);
  slash = strrchr(path, '/');

  if (!slash)
    return git__throw(GIT_EINVALIDARGS, "The path doesn't have slashes");

  memset(oid, 0x0, sizeof(oid));
  git_oid_fmt(oid, git_indexer_hash(idx));
  ret = sprintf(slash + 1, "pack-%s.pack", oid);
  if(ret < 0)
    return git__throw(GIT_EOSERR, "Failed to generate pack name");

  printf("Renameing pack to %s\n", path);
  return rename(packname, path);
}

int fetch(git_repository *repo, int argc, char **argv)
{
  git_remote *remote = NULL;
  git_config *cfg = NULL;
  git_indexer *idx = NULL;
  git_indexer_stats stats;
  int error;
  char *packname = NULL;

  error = git_repository_config(&cfg, repo, NULL, NULL);
  if (error < GIT_SUCCESS)
    return error;

  printf("Fetching %s\n", argv[1]);
  error = git_remote_get(&remote, cfg, argv[1]);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_connect(remote, GIT_DIR_FETCH);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_negotiate(remote);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_download(&packname, remote);
  if (error < GIT_SUCCESS)
    return error;

  printf("The packname is %s\n", packname);

  error = git_indexer_new(&idx, packname);
  if (error < GIT_SUCCESS)
    return error;

  /* This should be run in paralel, but it'd be too complicated right now */

  error = git_indexer_run(idx, &stats);
  if (error < GIT_SUCCESS)
    return error;

  printf("Received %d objects\n", stats.total);

  error = git_indexer_write(idx);
  if (error < GIT_SUCCESS)
    return error;

  error = rename_packfile(packname, idx);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_update_tips(remote);
  if (error < GIT_SUCCESS)
    return error;

  free(packname);
  git_indexer_free(idx);
  git_remote_free(remote);
  git_config_free(cfg);

  return GIT_SUCCESS;
}
