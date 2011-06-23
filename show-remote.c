#include <git2.h>
#include <stdio.h>
#include <stdlib.h>

int show_remote(git_repository *repo, int argc, char ** argv)
{
  git_remote *remote;
  git_config *cfg;
  const git_refspec *refspec;
  int error;

  error = git_repository_config(&cfg, repo, NULL, NULL);
  if (error < GIT_SUCCESS)
    return error;

  error = git_remote_get(&remote, cfg, argv[1]);
  if (error < GIT_SUCCESS)
    return error;

  printf("%s \t%s\n", git_remote_name(remote), git_remote_url(remote));
  refspec = git_remote_fetchspec(remote);
  printf("fetch: %s -> %s\n", git_refspec_src(refspec),
	 git_refspec_dst(refspec));
  printf("%d\n", git_refspec_src_match(refspec, "refs/heads/master"));

  git_remote_free(remote);
  git_config_free(cfg);

  return GIT_SUCCESS;
}
