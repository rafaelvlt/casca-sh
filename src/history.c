#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "history.h"

#define BUFFER_SIZE 1024

static History history;

void init_history(){
  char* env_variable = getenv("HOME");
  char history_filename[] = ".casca_history";
  int history_path_size = strlen(env_variable) + strlen(history_filename) + 2;

  history.path = (char*)malloc(history_path_size);
  snprintf(history.path, history_path_size, "%s/%s", env_variable, history_filename);

  // check for file existence
  FILE* fstream = NULL;
  if (access(history.path, F_OK | R_OK) == 0){

    fstream = fopen(history.path, "r");
    if (fstream == NULL){
      printf("ERROR: failed to open history file\n");
      return;
    }
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, sizeof(buffer), fstream) != NULL){
      buffer[strcspn(buffer, "\n")] = '\0';
      int index_to_write = history.history_count % HISTORY_SIZE;
      if (history.history_array[index_to_write].input != NULL){
        free(history.history_array[index_to_write].input);
      }
      history.history_array[index_to_write].input = strdup(buffer);
      history.history_count++;
    }
  }
  //creates file
  else{
    fstream = fopen(history.path, "a");
    if (fstream == NULL){
      printf("ERROR: failed to create history file\n");
      return;
    }
  }
    
  if (fstream != NULL) fclose(fstream);
}

void add_to_history(char* user_input){
  FILE* fstream = fopen(history.path, "a");
  if (fstream == NULL){
    printf("ERROR: failed to open history file to read last input\n");
    return;
  }
  unsigned long index_to_write = history.history_count % HISTORY_SIZE;
  if (history.history_array[index_to_write].input != NULL){
    free(history.history_array[index_to_write].input);
  }
  history.history_array[index_to_write].input = strdup(user_input);
  fprintf(fstream, "%s\n", user_input);
  history.history_count++;

  fclose(fstream);
}

void print_history(){
  unsigned int start_idx = (history.history_count < HISTORY_SIZE) ? 0 : history.history_count % HISTORY_SIZE;
  for (unsigned int i = 0; i < HISTORY_SIZE && i < history.history_count; i++){
    unsigned int index = (start_idx + i) % HISTORY_SIZE;
    printf("%d %s\n", i+1, history.history_array[index].input);
  }
}

void sync_history(){
  FILE* fstream = fopen(history.path, "w");
  if (fstream == NULL){
    printf("ERROR: failed to open history file sync with array\n");
    return;
  }
  
  unsigned int start_idx = (history.history_count < HISTORY_SIZE) ? 0 : history.history_count % HISTORY_SIZE;
  for (unsigned int i = 0; i < HISTORY_SIZE && i < history.history_count; i++){
    unsigned int index = (start_idx + i) % HISTORY_SIZE;
    if (history.history_array[index].input != NULL){
      fprintf(fstream, "%s\n", history.history_array[index].input);
    }
  }

  fclose(fstream);
}

void clear_history(){
   
}

void free_history(){
  for (unsigned long i = 0; i < HISTORY_SIZE && i < history.history_count; i++){
    free(history.history_array[i].input);
  }
  free(history.path);
}
