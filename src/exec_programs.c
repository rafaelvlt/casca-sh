#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parsing.h"
#include "built_in.h"
#include "exec_programs.h"

extern char** environ;

void handle_command(char* command, char** args){
  bool success = handle_built_in(command, args); 
  if (!success){
    execute_external_program(command, args);
  }
}

void execute_external_program(char* command, char** args){
      char* command_path = NULL;
      command_path = search_program(command); 

      if (command_path != NULL){

        pid_t c_process = fork();

        if (c_process == -1){
          printf("ERROR: Failed to fork\n");
          free(command_path);
        }
        else if(c_process == 0){

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
