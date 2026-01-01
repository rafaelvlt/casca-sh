#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

extern char** environ;

#define BUILT_IN_AMOUNT 3 
#define BUILT_IN_MAX_SIZE 10
#define COMMAND_PATH_MAX_LENGTH 256

char** parse_args(char* input){
    
  int capacity = 5;
  int* args_length = (int*)malloc(sizeof(int) * capacity);
  if (args_length == NULL){
    printf("ERROR: Failed to allocate array of args length\n");
    return NULL;
  }
  int arg_amount = 0;
  char* cursor = input;
  while (*cursor != '\0'){
    while(*cursor == ' '){
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
      while(*aux_cursor != ' ' && *aux_cursor != '\0'){
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

    while(*cursor == ' ' && *cursor != '\0'){
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

char* search_program(char* path_dest, const char* command){
  bool found = false;

  char* path_variable = strdup(getenv("PATH"));
  char* current_path = strtok(path_variable, ":"); 

  path_dest = (char*)malloc(sizeof(char) * COMMAND_PATH_MAX_LENGTH);

  while (current_path != NULL && !found){
    strcpy(path_dest, current_path);
    strcat(path_dest, "/");
    strcat(path_dest, command);

    // syscall to check execute permission and file existence
    if (access(path_dest, X_OK) == 0){
      found = true;
    }
    else{
      current_path = strtok(NULL, ":");
    }
  }
  free(path_variable);
  return path_dest;
}

void set_built_in(char built_in_array[BUILT_IN_AMOUNT][BUILT_IN_MAX_SIZE]){
  strcpy(built_in_array[0], "exit");
  strcpy(built_in_array[1], "echo");
  strcpy(built_in_array[2], "type");
}

void type_command(char** args, char built_in[BUILT_IN_AMOUNT][BUILT_IN_MAX_SIZE]){
  
  char* command = args[1];

  bool is_built_in = false;
  for (int i = 0; i < BUILT_IN_AMOUNT && !is_built_in; i++){
    if (strcmp(command, built_in[i]) == 0){
      is_built_in = true;
    }
  }

  if (is_built_in){
    printf("%s is a shell builtin\n", command);
  }
  else{
    char* command_path = NULL;
    command_path = search_program(command_path, command);

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
      printf("%s\n", args[idx]);
  }
}


int main(){
  setbuf(stdout, NULL);
  bool repl_on = true;

  char built_in[BUILT_IN_AMOUNT][BUILT_IN_MAX_SIZE];
  set_built_in(built_in);
  
  while (repl_on){
    printf("$ ");

    char user_input[1024];
    fgets(user_input, sizeof(user_input), stdin); 
    user_input[strcspn(user_input, "\n")] = '\0';
    char** args = parse_args(user_input);
    char* command = args[0];
     
    bool command_is_builtin = true;
    if (args[0] == NULL){
      ;
    }
    else if (strcmp(command, "exit") == 0){
      repl_on = false;
    }
    else if(strcmp(command, "echo") == 0){
      echo_command(args);
    }
    else if(strcmp(command, "type") == 0){
      type_command(args, built_in);
    }
    else{
      command_is_builtin = false;
    }

    if (!command_is_builtin){
      char* command_path = NULL;
      command_path = search_program(command_path, command); 

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
    free_args(args);
  }

  return EXIT_SUCCESS;
}
