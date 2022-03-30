#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

void
pingpong()
{
  int p1[2], p2[2];
  int pid ;

  if (pipe(p1) < 0 || pipe(p2) < 0)
    panic("pipe");

  pid = fork();

  if (pid == 0) {
    char c;
    int my_pid = getpid();
    // close these fd because child process doesn't need write into p1[1] and read from p2[0]
    close(p1[1]);
    close(p2[0]);

    read(p1[0] , &c, 1);
    fprintf(1, "%d : received ping %c \n", my_pid, c);
    c = 'C'; // child
    write(p2[1] , &c, 1);

    close(p1[0]);
    close(p2[1]);
  }else if (pid > 0){
    char c = 'P'; // parent
    int my_pid = getpid();
    close(p1[0]);
    close(p2[1]);

    write(p1[1], &c, 1);
    read(p2[0] , &c, 1);
    fprintf(1, "%d : received pong %c\n", my_pid, c);

    close(p1[1]);
    close(p2[0]);
  }else{
    panic("fork");
  }
}

int
main(int argc, char *argv[])
{
  pingpong();
  exit(0);
}
