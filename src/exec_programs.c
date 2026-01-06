#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parsing.h"
#include "built_in.h"
#include "exec_programs.h"
#include "redirection.h"

extern char** environ;

void handle_command(char* command, char** args){
  info_redirection redirection = check_for_redirection(args);
  int backup_stdout = 0;
  int outfile_fd = 0;

  if (redirection.fd == FD_STDOUT){
    backup_stdout = dup(FD_STDOUT);
    outfile_fd = open(redirection.target_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    dup2(outfile_fd, FD_STDOUT);
  }

  bool success = handle_built_in(command, args); 

  if (redirection.fd == FD_STDOUT){
    dup2(backup_stdout, FD_STDOUT);
    close(backup_stdout);
    close(outfile_fd);
  }
  if (!success){
    execute_external_program(command, args, redirection);
  }


  if (redirection.fd != NO_REDIRECTION){
    free(redirection.target_path);
  }
}

void execute_external_program(char* command, char** args, info_redirection redirection){
  char* command_path = NULL;
  command_path = search_program(command); 

  if (command_path != NULL){

    pid_t c_process = fork();

    if (c_process == -1){
      printf("ERROR: Failed to fork\n");
      free(command_path);
    }
    else if(c_process == 0){
      if (redirection.fd == FD_STDOUT){
        int outfile_fd = open(redirection.target_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        dup2(outfile_fd, FD_STDOUT);
        close(outfile_fd);
      }
      execve(command_path, args, environ);
      free_args(args);
      free(command_path);
      exit(EXIT_SUCCESS);
    }
    else{
      int status;
      waitpid(c_process, &status, 0);
      free(command_path);
    }
  }
  else{
    printf("%s: command not found\n", command);

  }
}


char* search_program(const char* command){
  bool found = false;

  char* path_variable = strdup(getenv("PATH"));
  //entire path(even if its only one) + program size + '\0' and /
  int buffer_size = strlen(path_variable) + strlen(command) + 2;
  char* current_path = strtok(path_variable, ":"); 

  char* path_dest = (char*)malloc(sizeof(char) * buffer_size);

  while (current_path != NULL && found == false){
    snprintf(path_dest, buffer_size, "%s/%s", current_path, command);

    // syscall to check execute permission and file existence
    if (access(path_dest, X_OK) == 0){
      found = true; 
    }
    else{
      current_path = strtok(NULL, ":");
    }
  }
  free(path_variable);

  if (found){
    return path_dest;
  }
  else{
    free(path_dest);
    return NULL;
  }
}
