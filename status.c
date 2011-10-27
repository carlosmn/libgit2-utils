#include <git2.h>

#include "common.h"

int status_cb(const char *path, unsigned int mode, void *data)
{
	puts(path);

	return GIT_SUCCESS;
}

int status(git_repository *repo, int argc, char **argv)
{
	return git_status_foreach(repo, status_cb, NULL);
}
