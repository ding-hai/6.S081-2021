#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void
primes(int parent_fd)
{
  int val;
  if(read(parent_fd, &val, sizeof(int)) == 0){
    exit(0);
  }
  
  fprintf(1, "prime %d\n", val); 

  int child_pipe[2];
  pipe(child_pipe);
  if(fork() > 0){
    int n;
    close(child_pipe[0]);
    while(read(parent_fd, &n, sizeof(int)) != 0) {
      if(n % val == 0){
        // fprintf(1, "drop %d\n", n);
        continue;
      }
      write(child_pipe[1], &n, sizeof(int));
    }
    close(child_pipe[1]);
  }else{
    close(child_pipe[1]);
    primes(child_pipe[0]);
  }
  wait(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int root_pipe[2];

  pipe(root_pipe);

  if(fork() > 0) {
    close(root_pipe[0]);
    for(int i=2; i<=35; i++){
      write(root_pipe[1], &i, sizeof(int));
    }
    close(root_pipe[1]);
  }else{
    close(root_pipe[1]);
    primes(root_pipe[0]);
  }
  wait(0);
  exit(0);
}
