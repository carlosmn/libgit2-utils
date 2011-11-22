
#include <git2.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

int diff_callback(const git_tree_diff_data *diff, void *data)
{
	char mod = 'E', oldoid[GIT_OID_HEXSZ + 1] = {0}, newoid[GIT_OID_HEXSZ + 1] = {0};

	git_oid_fmt(oldoid, &diff->old_oid);
	git_oid_fmt(newoid, &diff->new_oid);
	switch (diff->status) {
	case GIT_STATUS_ADDED:
		mod = 'A';
		break;
	case GIT_STATUS_MODIFIED:
		mod = 'M';
		break;
	case GIT_STATUS_DELETED:
		mod = 'D';
		break;
	}
	printf(":%06o %06o %s %s %c\t%s\n", diff->old_attr, diff->new_attr,
	       oldoid, newoid, mod, diff->path);

	return 0;
}

int diff_tree(git_repository *repo, int argc, char **argv)
{
	git_oid old, new;
	git_tree *told, *tnew;
	int error;

	git_oid_fromstr(&old, argv[1]);
	git_oid_fromstr(&new, argv[2]);

	error = git_tree_lookup(&told, repo, &old);
	if (error < GIT_SUCCESS)
	    return error;
	error = git_tree_lookup(&tnew, repo, &new);
	if (error < GIT_SUCCESS)
	    return error;

	error = git_tree_diff(told, tnew, diff_callback, NULL);

	git_tree_close(told);
	git_tree_close(tnew);

	return error;
}
