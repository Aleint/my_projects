// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid=fork();
  if( pid > 0){
    printf("zombie process pid: %d\n", pid);
    sleep(5);  // Let child exit before parent.
  }
  
  exit(0);
}
