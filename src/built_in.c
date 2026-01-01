#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"
#include "built_in.h"

static const Built_In built_in_array[] = {
                  {echo_command, "echo"},
                  {type_command, "type" },
                  {exit_command, "exit"},
                  {NULL, NULL}
};

bool handle_built_in(char* command, char** args){
  int idx = 0;
  while (built_in_array[idx].name != NULL){
    if (strcmp(built_in_array[idx].name, command) == 0){
      built_in_array[idx].built_in_function(args); 
      return true;
    }
    idx++;
  }
  return false;
}

void type_command(char** args){
  
  char* command = args[1];

  bool is_built_in = false;
  int idx = 0;
  while (built_in_array[idx].name != NULL && !is_built_in){
    if (strcmp(command, built_in_array[idx].name) == 0){
      is_built_in = true;
    }
    idx++;
  }

  if (is_built_in){
    printf("%s is a shell builtin\n", command);
  }
  else{
    char* command_path = NULL;
    command_path = search_program(command);

    if (command_path != NULL){
      printf("%s is %s\n", command, command_path);
      free(command_path);
    }
    else{
      printf("%s: not found\n", command);
    }
  }
}

void echo_command(char** args){
  int idx = 1;
  while (args[idx] != NULL){
    if (args[idx+1] != NULL){
      printf("%s ", args[idx]);
    }
    else{
      printf("%s\n", args[idx]);
    }

    idx++;
  }
}

void exit_command(char** args){
  int exit_code = EXIT_SUCCESS;
  if (args[1] != NULL){
    exit_code = atoi(args[1]);
  }
  free_args(args);
  exit(exit_code);
}
