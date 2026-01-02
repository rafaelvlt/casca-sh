#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parsing.h"

char** get_user_input(){
  char* user_input = NULL;
  unsigned int capacity = 128;
  user_input = (char*)malloc(capacity);

  unsigned int input_length = 0;
  int buff = fgetc(stdin);
  while (buff != EOF && buff != '\n') {
    if (input_length + 1 >= capacity){
      capacity *= 2;
      char* temp = realloc(user_input, capacity);

      if (temp == NULL){
        printf("ERROR: failed to reallocate input buffer\n");
        free(user_input);
        return NULL;
      }

      user_input = temp;
    }
    if (buff != EOF){
      user_input[input_length++] = buff;
    }
    buff = fgetc(stdin);
  }
  user_input[input_length] = '\0';
  char** args = parse_args(user_input, ' ');

  free(user_input);

  return args;
}

char** parse_args(char* input, char delim){
    
  int capacity = 5;
  int* args_length = (int*)malloc(sizeof(int) * capacity);
  if (args_length == NULL){
    printf("ERROR: Failed to allocate array of args length\n");
    return NULL;
  }
  int arg_amount = 0;
  char* cursor = input;
  while (*cursor != '\0'){
    while(*cursor == delim){
      cursor++;
    }
    if (*cursor != '\0'){
      if (arg_amount+1 == capacity){
        capacity *= 2;
        int* temp = (int*)realloc(args_length, sizeof(int) * capacity); 
        if (temp == NULL){
          free(args_length);
          printf("ERROR: Failed to reallocate for args length");
          return NULL;
        }
        args_length = temp;
      }
      int len = 0;
      char* aux_cursor = cursor;
      while(*aux_cursor != delim && *aux_cursor != '\0'){
        aux_cursor++;
        len++;
      }
      args_length[arg_amount] = len;
      arg_amount++;
      cursor = aux_cursor;
    }
  }

  char** args = (char**) malloc(sizeof(char*) * (1 + arg_amount));
  if (args == NULL){
    printf("ERROR: Failed to allocate memory for args array\n");
    free(args_length);
    return NULL;
  }

  cursor = input;
  for (int i = 0; i < arg_amount; i++){
    args[i] = (char*)malloc(sizeof(char) * (1 + args_length[i]));
    if (args[i] == NULL){
      printf("ERROR: Failed to allocate memory for an arg\n");
      for (int j = i; j >= 0; j--){
        free(args[j]);
      }
      free(args);
      free(args_length);
      return NULL;
    }

    while(*cursor == delim && *cursor != '\0'){
      cursor++;
    }
    strncpy(args[i], cursor, args_length[i]);
    args[i][args_length[i]] = '\0';
    cursor = cursor + args_length[i];
  }
  //NULL finisher
  args[arg_amount] = NULL;
  free(args_length);
  return args;
} 

void free_args(char** args){
  int idx = 0;
  while(args[idx] != NULL){
    free(args[idx]);
    idx++;
  }
  free(args);
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

char* process_prompt_cwd(char* cwd){
  char* home = getenv("HOME");
  char* substr_ptr = strstr(cwd, home);

  if (substr_ptr == NULL){
    return cwd;
  }
  else{
    // +2 for null terminator and tilde
    char* print_cwd = (char*)malloc(strlen(cwd) - strlen(home) + 2);
    int idx = 0;
    print_cwd[idx++] = '~';
    for (int i = strlen(home); i < (int)strlen(cwd); i++, idx++){
      print_cwd[idx] = cwd[i];
    }
    print_cwd[idx] = '\0';
    free(cwd);
    return print_cwd;
  }
}

