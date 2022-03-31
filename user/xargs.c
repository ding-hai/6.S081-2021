#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXBUFFER 1024
#define is_blank(c) (c == ' ' || c == '\t')
#define is_new_line(c) (c=='\n')

int
run_cmd(char *argv[]) {
  if (fork() == 0) {
    return exec(argv[0], argv);
  } else {
    int status;
    wait(&status);
    return status;
  }
}

void reset(char *new_argv[], int start_idx) {
  for (int i = start_idx; i < MAXARG; ++i) {
    new_argv[i] = 0;
  }
}


int parse_new_line(char *buffer, int buffer_offset, int buffer_length, char **argv, int argv_offset) {
  char *p = 0;
  char c = '\0';
  for (int i = buffer_offset; i < buffer_length; ++i) {
    c = buffer[i];
    if (is_blank(c) || is_new_line(c)) {
      if (p) {
        argv[argv_offset++] = p;
        p = 0;
      }
      buffer[i] = '\0';
      if (is_new_line(c)) {
        return i;
      }
    } else if (!p) {
      p = buffer + i;
    }
  }
  if (!is_blank(c) && !is_new_line(c) && p)
    argv[argv_offset++] = p;
  return buffer_length;
}


int
main(int argc, char *argv[]) {

  char buffer[MAXBUFFER];
  int buffer_length = 0, buffer_offset = 0;
  char *new_argv[MAXARG];
  int new_argv_offset = 0;
  int n = 0;
  while ((n = read(0, buffer + buffer_length, MAXBUFFER - buffer_length)) > 0)
    buffer_length += n;
  buffer[buffer_length] = '\0';
//  printf("len=%d\t%s\n", buffer_length, buffer);

  for (int i = 1; i < argc; ++i) {
//    printf("%s\n", argv[i]);
    new_argv[new_argv_offset++] = argv[i];
  }

  while (buffer_offset < buffer_length) {
    if (!is_new_line(buffer[buffer_offset])) {
      reset(new_argv, new_argv_offset);
      buffer_offset = parse_new_line(buffer, buffer_offset, buffer_length, new_argv, new_argv_offset);
//      printf("%d\n", buffer_offset);
      run_cmd(new_argv);
    }
    buffer_offset++;
  }
  exit(0);
}
