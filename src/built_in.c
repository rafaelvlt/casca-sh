#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "history.h"
#include "parsing.h"
#include "built_in.h"
#include "exec_programs.h"

static Built_In built_in_array[] = {
                  {echo_command, "echo"},
                  {pwd_command, "pwd"},
                  {cd_command, "cd"},
                  {type_command, "type" },
                  {history_command, "history"},
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

void autocomplete_load_built_in(Trie* dictionary){
  for(int i = 0; built_in_array[i].name != NULL; i++){
    trie_insert(dictionary, built_in_array[i].name);
  }
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

void pwd_command([[maybe_unused]]char** args){
  char* cwd = getcwd(NULL, 0);
  printf("%s\n", cwd);
  free(cwd);
}

static char* replace_tilde(char* path, char* home){
  unsigned int new_size = strlen(path) + strlen(home) + 1;
  char* final_dir = (char*)malloc(new_size);
  memset(final_dir, '\0', new_size);
  
  int tilde_counter = 0;
  int idx = 0;
  for (int i = 0; path[i] != '\0'; i++){
    if (path[i] != '~'){
      final_dir[idx] = path[i]; 
      idx++;
    }
    else{
      tilde_counter++;
      if (tilde_counter > 1){
        new_size = strlen(path) + (strlen(home) * tilde_counter) + 1;
        char* temp = realloc(final_dir, new_size);
        if (temp == NULL){
          printf("ERROR: failed to reallocate for new path\n");
          free(final_dir);
          return NULL;
        }
        final_dir = temp;
      }
      // prefix slash or not
      int j;
      if (idx == 0){
        j = 0;
      }
      else{
        j = 1;
      }
      for (; j < (int)strlen(home); j++){
        final_dir[idx] = home[j];
        idx++;
      }
      
    }
  }
  
  return final_dir;
}

void cd_command(char** args){
  if (args[2] != NULL){
    printf("cd: too many arguments\n");
    return ;
  }

  char* home = getenv("HOME");
  if (args[1] == NULL){
    if (home != NULL){
      int chdir_status = chdir(home);
      if (chdir_status != 0){
        printf("ERROR: error while changing dir\n");
        return;
      }
    } 
    else{
      printf("ERROR: no home env variable found\n");
      return ;
    }
  }
  char* target_dir = args[1];
  target_dir = replace_tilde(target_dir, home);

  struct stat dir_info;
  int dir_status = stat(target_dir, &dir_info);
    
  // checks if its a directory and there's not an error
  if (dir_status == 0 && (dir_info.st_mode & S_IFDIR)){
    int chdir_status = chdir(target_dir);
    if (chdir_status != 0){
      printf("ERROR: error while changing dir\n");
    }
  }
  else{
    printf("cd: %s: No such file or directory\n", target_dir);
  }
  free(target_dir);
}

void history_command([[maybe_unused]] char** args){
  print_history();
}

void exit_command(char** args){
  int exit_code = EXIT_SUCCESS;
  if (args[1] != NULL){
    exit_code = atoi(args[1]);
  }
  free_args(args);
  free_history();
  exit(exit_code);
}
