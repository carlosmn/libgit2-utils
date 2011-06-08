#include <git2.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

int parse_pkt_line(git_repository *repo, int argc, char **argv)
{
  char *line = argv[1];
  char *after = line, oid[GIT_OID_HEXSZ+1] = {0};
  int error, len;
  git_pkt *pkt;

  /*
   * Assume that the user has piped a file in
   */
  fseek(stdin, 0, SEEK_END);
  len = ftell(stdin);
  fseek(stdin, 0, SEEK_SET);

  line = malloc(len+1);
  if(line == NULL)
    return GIT_ENOMEM;

  memset(line, 0, len+1);
  fread(line, len, sizeof(char), stdin);
  after = line;

  while(*after != '\0'){
	  error = git_pkt_parse_line(&pkt, line, &after);
	  if (error < GIT_SUCCESS)
		  return error;

	  line = after;

	  switch(pkt->type){
	  case GIT_PKT_REF:
		  {
			  git_remote_head *h = &(((git_pkt_ref *)pkt)->head);
			  const char *caps = ((git_pkt_ref *)pkt)->capabilities;
			  //puts("Found a ref pkt");
			  git_oid_fmt(oid, &h->oid);
			  printf("%s\t%s\n", oid, h->name);
			  if(caps != NULL)
			    printf("    Capabilities: %s\n", caps);
			  break;
		  }
	  case GIT_PKT_FLUSH:
		  puts("flush! do something!");
	  default:
		  printf("default: %d\n", *after);
	  }
  }
 
  return error;
}
