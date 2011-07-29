#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

int index_cb(const git_indexer_stats *stats, void *data)
{
  printf("\rProcessing %d of %d", stats->processed, stats->total);
}

int index_pack(git_repository *repo, int argc, char **argv)
{
  git_indexer *indexer;
  int error;
  char hash[GIT_OID_HEXSZ + 1] = {0};

  if (argc < 2) {
    fprintf(stderr, "I need a packfile\n");
    return EXIT_FAILURE;
  }

  error = git_indexer_new(&indexer, argv[1]);
  if (error < GIT_SUCCESS)
    return error;

  error = git_indexer_run(indexer, index_cb, NULL);
  if (error < GIT_SUCCESS)
    return error;

  puts("");
  error = git_indexer_write(indexer);

  git_oid_fmt(hash, git_indexer_result(indexer));
  puts(hash);

  git_indexer_free(indexer);

  return GIT_SUCCESS;
}
