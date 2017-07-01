#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
  char *vp[] = {
    "-ex",
    "set pagination 0",
    "-ex",
    "thread apply all bt",
    "--batch",
    "-p",
    argv[1],
  };
  
  if(argc != 2){
    return;
  }

  execvp("/usr/bin/gdb\0",
	 vp);
  
  return(0);
}
