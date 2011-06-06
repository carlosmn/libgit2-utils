#include <git2.h>
#include "common.h"

int parse_pkt_line(git_repository *repo, int argc, char **argv)
{
  char *line = argv[1];
  char *after, oid[GIT_OID_HEXSZ+1] = {0};
  int error;
  git_pkt *pkt;

  error = git_pkt_parse_line(&pkt, line, &after);
  if (error < GIT_SUCCESS)
    return error;

  switch(pkt->type){
  case GIT_PKT_REF:
    {
      git_remote_head *h = &(((git_pkt_ref *)pkt)->head);
      puts("Found a ref pkt");
      git_oid_fmt(oid, &h->oid);
      printf("%s\t%s\n", oid, h->name);
      break;
    }
  default:
    return GIT_EOBJCORRUPTED;
  }
 
  return error;
}
