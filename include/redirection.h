#ifndef REDIRECTION_H
#define REDIRECTION_H
#include <fcntl.h>

#define NO_REDIRECTION -1
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

typedef struct{
  int fd;
  char* target_path;
  int open_flags;
  mode_t mode_flags;
} info_redirection;

info_redirection check_for_redirection(char** args);
void output_stream_redirection(char* target);

#endif 
