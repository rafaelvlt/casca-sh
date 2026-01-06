#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "redirection.h"


info_redirection check_for_redirection(char** args){
  info_redirection info = {NO_REDIRECTION, NULL, 0, 0};
  int idx = 0;
  for (; args[idx] != NULL && info.fd == NO_REDIRECTION; idx++){
    if (!strcmp(args[idx], ">") || !strcmp(args[idx], "1>")){
      info.fd = FD_STDOUT;    
      info.target_path = strdup(args[idx+1]);
      info.open_flags = O_WRONLY | O_CREAT | O_TRUNC;
      info.mode_flags = S_IRUSR | S_IWUSR;
      free(args[idx]);
      free(args[idx+1]);
      args[idx] = NULL;
      args[idx+1] = NULL;
    }
    else if (!strcmp(args[idx], ">>") || !strcmp(args[idx], "1>>")){
      info.fd = FD_STDOUT;    
      info.target_path = strdup(args[idx+1]);
      info.open_flags = O_WRONLY | O_CREAT | O_APPEND;
      info.mode_flags = S_IRUSR | S_IWUSR;
      free(args[idx]);
      free(args[idx+1]);
      args[idx] = NULL;
      args[idx+1] = NULL;
    }
    else if (!strcmp(args[idx], "2>")){
      info.fd = FD_STDERR;    
      info.target_path = strdup(args[idx+1]);
      info.open_flags = O_WRONLY | O_CREAT | O_TRUNC;
      info.mode_flags = S_IRUSR | S_IWUSR;
      free(args[idx]);
      free(args[idx+1]);
      args[idx] = NULL;
      args[idx+1] = NULL;
    }
    else if (!strcmp(args[idx], "2>>")){
      info.fd = FD_STDERR;    
      info.target_path = strdup(args[idx+1]);
      info.open_flags = O_WRONLY | O_CREAT | O_APPEND;
      info.mode_flags = S_IRUSR | S_IWUSR;
      free(args[idx]);
      free(args[idx+1]);
      args[idx] = NULL;
      args[idx+1] = NULL;
    }
    else if (!strcmp(args[idx], "<")){
      info.fd = FD_STDIN;
      info.target_path = strdup(args[idx+1]);
      info.open_flags = O_WRONLY | O_CREAT | O_TRUNC;
      info.mode_flags = S_IRUSR | S_IWUSR;
      free(args[idx]);
      free(args[idx+1]);
      args[idx] = NULL;
      args[idx+1] = NULL;
    }
  }
  return info;
}

void output_stream_redirection(char* target){

  int target_fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  dup2(target_fd, FD_STDOUT);
  close(target_fd);
}
