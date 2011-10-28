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
	git_object *obja, *objb;
	git_tree *told, *tnew;
	int error;

	if (argc < 3) {
		fprintf(stderr, "usage: diff-tree old new\n");
		return GIT_ERROR;
	}

	git_oid_fromstr(&old, argv[1]);
	git_oid_fromstr(&new, argv[2]);

	error = git_object_lookup(&obja, repo, &old, GIT_OBJ_ANY);
	if (error < GIT_SUCCESS)
	    return error;
	error = git_object_lookup(&objb, repo, &new, GIT_OBJ_ANY);
	if (error < GIT_SUCCESS)
	    return error;

	switch (git_object_type(obja)) {
	case GIT_OBJ_TREE:
		told = (git_tree *) obja;
		break;
	case GIT_OBJ_COMMIT:
		git_commit_tree(&told, (git_commit *) obja);
		git_object_close(objb);
		break;
	default:
		fprintf(stderr, "I can't do anything with the 1st OID");
		break;
	}

	switch (git_object_type(objb)) {
	case GIT_OBJ_TREE:
		tnew = (git_tree *) objb;
		break;
	case GIT_OBJ_COMMIT:
		git_commit_tree(&tnew, (git_commit *) objb);
		git_object_close(objb);
		break;
	default:
		fprintf(stderr, "I can't do anything with the 2nd OID");
		break;
	}

	error = git_tree_diff(told, tnew, diff_callback, NULL);

	git_tree_close(told);
	git_tree_close(tnew);

	return error;
}
