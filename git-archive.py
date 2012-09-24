#!/usr/bin/env python

import tarfile, os, argparse
import pygit2 as git
from cStringIO import StringIO

def add_file(tar, entry, path = None):
    object = entry.to_object().read_raw()
    name = path if path != None else entry.name
    info = tarfile.TarInfo(name)
    info.size = len(object)
    info.mtime = timestamp
    info.uname = info.gname = 'root' # For compatability with git
    if entry.attributes == 0120000:
        info.type = tarfile.SYMTYPE
        info.linkname = object
        info.mode = 0777

    tar.addfile(info, StringIO(object))

def add_dir(tar, tentry, path = []):
    tree = repo[tentry.oid]
    path.append(tentry.name)
    for entry in tree:
        if entry.attributes == 040000:
            add_dir(out, entry, path)
        else:
            path.append(entry.name)
            add_file(out, entry, "/".join(path))

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('treeish', metavar='tree-ish', default='HEAD', nargs='?', help='Tree-ish to package')
    parser.add_argument('-o', '--output', help='Select the filename')
    return parser.parse_args()

args = parse_args()
repo = git.Repository('.')

# FIXME: This assumes a ref pointing to a commit
oid = repo.lookup_reference(args.treeish).resolve().oid
commit = repo[oid]
timestamp = commit.committer.time
tree = commit.tree

filename = args.output
if filename == None:
    filename = "%s.tar" % (args.treeish)

out = tarfile.open(filename, mode='w')

for entry in tree:
    if entry.attributes == 040000:
        add_dir(out, entry)
    else:
        add_file(out, entry)

out.close()
