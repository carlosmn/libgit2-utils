#!/usr/bin/env python

import tarfile, os, argparse
import pygit2 as git
from cStringIO import StringIO
from pygit2 import GIT_FILEMODE_LINK as SYMLINK

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('treeish', metavar='tree-ish', default='HEAD', nargs='?', help='Tree-ish to package')
    parser.add_argument('-o', '--output', help='Select the filename')
    return parser.parse_args()

args = parse_args()
repo = git.Repository('.')

# FIXME: This assumes a ref pointing to a commit
oid = repo.lookup_reference(args.treeish).resolve().target
commit = repo[oid]
timestamp = commit.committer.time
tree = commit.tree

filename = args.output
if filename == None:
    filename = "%s.tar" % (args.treeish)

out = tarfile.open(filename, mode='w')

index = git.Index()
index.read_tree(tree)

for entry in index:
    content = repo[entry.id].read_raw()
    info = tarfile.TarInfo(entry.name)
    info.size = len(content)
    info.mtime = timestamp
    info.uname = info.gname = 'root' # just because git does this
    if entry.mode == SYMLINK:
        info.type = tarfile.SYMTYPE
        info.linkname = content
        info.mode = 0777 # symlinks get placeholder

    tar.addfile(info, StringIO(content))

out.close()
