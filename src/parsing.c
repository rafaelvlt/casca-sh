#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parsing.h"


typedef enum{
  NORMAL = 0,
  READING,
  WRITING,
  SQUOTE,
  DQUOTE,
  END
} CursorState;

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

static void parse_normal_state(char** cursor_ptr, CursorState* state_ptr, char delim){
      if (**cursor_ptr != delim){
        if (**cursor_ptr == '\0'){
          *state_ptr = END;
        } 
        else if (**cursor_ptr == '\''){
          *state_ptr = SQUOTE;
          (*cursor_ptr)++;
        }
        else if (**cursor_ptr == '"'){
          *state_ptr = DQUOTE;
          (*cursor_ptr)++;
        }
        else{
          *state_ptr = READING;
        }
      }
      else{
        (*cursor_ptr)++;
      }
}
static char** reallocate_args(int* args_capacity_ptr, char*** args_array_ptr, int* args_amount_ptr){
  (*args_capacity_ptr) *= 2;
  char** temp = realloc(*args_array_ptr, sizeof(char*) * (*args_capacity_ptr));
  if (temp == NULL){
    printf("ERROR: error while reallocating for args array\n");
    for (int i = 0; i < *args_amount_ptr; i++){
      free(*args_array_ptr[i]);
    }
    free(*args_array_ptr);
    return NULL;
  }
  else{
    *args_array_ptr = temp;
  }
  return *args_array_ptr;
}

static void parse_writing_state(char** buffer_ptr, CursorState* state_ptr, char*** args_array_ptr,
                                int* buff_idx_ptr, int* args_amount_ptr, int* args_capacity_ptr)
{
  (*args_array_ptr)[*args_amount_ptr] = strdup(*buffer_ptr);

  if (*args_amount_ptr + 1 >= *args_capacity_ptr){
    (*args_array_ptr) = reallocate_args(args_capacity_ptr, args_array_ptr, args_amount_ptr);
    if ((*args_array_ptr) == NULL){
      free(*buffer_ptr);
      return;
    }
  }
  *buff_idx_ptr = 0;
  (*args_amount_ptr)++;
  *state_ptr = NORMAL;
}
static char* reallocate_buffer(char** buffer_ptr, int* buff_capacity_ptr, char*** args_array_ptr, int args_amount){
  (*buff_capacity_ptr) *= 2;
  char* temp = realloc((*buffer_ptr), (*buff_capacity_ptr) + 1);
  if (temp == NULL){
    printf("ERROR: Failed to reallocate arg (*buffer_ptr)\n");
    free((*buffer_ptr));
    for (int i = 0; i < args_amount; i++){
      free((*args_array_ptr)[i]);
    }
    free((*args_array_ptr));
    return NULL;
  }
  else{
    (*buffer_ptr) = temp;
  }
  return (*buffer_ptr);
}
static void parse_reading_state(char** cursor_ptr, CursorState* state_ptr, char delim,
                                char** buffer_ptr, int* buff_idx_ptr, int* buff_capacity_ptr,
                                char*** args_array_ptr, int args_amount)
{
  if (*(*cursor_ptr) == delim || *(*cursor_ptr) == '\0'){
    (*buffer_ptr)[(*buff_idx_ptr)] = '\0';
    *state_ptr = WRITING;
  }
  else{
    if (((*buff_idx_ptr) + 1) == *buff_capacity_ptr){
      (*buffer_ptr) = reallocate_buffer(buffer_ptr, buff_capacity_ptr, args_array_ptr, args_amount);
    }
    if ((**cursor_ptr) == '\\'){
      (*cursor_ptr)++;
      (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
    }
    else if (!((**cursor_ptr) == '\'' || (**cursor_ptr) == '"')){
      (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
    }
    (*cursor_ptr)++;
  }
}

static void parse_squote_state(char** cursor_ptr, CursorState* state_ptr,
                                char** buffer_ptr, int* buff_idx_ptr, int* buff_capacity_ptr,
                                char*** args_array_ptr, int args_amount)
{
  while((*state_ptr) == SQUOTE){
    if (*(*cursor_ptr) == '\''){
      if (*((*cursor_ptr) + 1) == '\''){
        (*cursor_ptr) += 2;
      }
      else{
        (*cursor_ptr)++;
        if ((*buff_idx_ptr) > 0){
          (*buffer_ptr)[(*buff_idx_ptr)] = '\0';
          (*state_ptr) = WRITING;
        }
        else{
          (*state_ptr) = NORMAL;
        }
      }
    }
    else{
      if (((*buff_idx_ptr) + 1) == *buff_capacity_ptr){
        (*buffer_ptr) = reallocate_buffer(buffer_ptr, buff_capacity_ptr, args_array_ptr, args_amount);
      }
      if ((**cursor_ptr) == '\\'){
        (*cursor_ptr)++;
        (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
      }
      else{
        (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
      }
      (*cursor_ptr)++;
    }
  }
}


static void parse_dquote_state(char** cursor_ptr, CursorState* state_ptr,
                                char** buffer_ptr, int* buff_idx_ptr, int* buff_capacity_ptr,
                                char*** args_array_ptr, int args_amount)
{
  while((*state_ptr) == DQUOTE){
    if (*(*cursor_ptr) == '"'){
      if (*((*cursor_ptr) + 1) == '"'){
        (*cursor_ptr) += 2;
      }
      else{
        (*cursor_ptr)++;
        if ((*buff_idx_ptr) > 0){
          (*buffer_ptr)[(*buff_idx_ptr)] = '\0';
          (*state_ptr) = WRITING;
        }
        else{
          (*state_ptr) = NORMAL;
        }
      }
    }
    else{
      if (((*buff_idx_ptr) + 1) == *buff_capacity_ptr){
        (*buffer_ptr) = reallocate_buffer(buffer_ptr, buff_capacity_ptr, args_array_ptr, args_amount);
      }
      if ((**cursor_ptr) == '\\'){
        char escape_sequence[] = "$\"\\\n";
        bool should_escape = false;
        for (char* escape_ptr = escape_sequence; *escape_ptr != '\0'; escape_ptr++){
          if (*(*cursor_ptr + 1) == *escape_ptr){
            should_escape = true;
          }
        }
        if (should_escape){
          (*cursor_ptr)++;
          (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
        }
        else{
          (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
        }
      }
      else{
        (*buffer_ptr)[(*buff_idx_ptr)++] = *(*cursor_ptr);
      }
      (*cursor_ptr)++;
    }
  }
}

char** parse_args(char* input, char delim){

  int args_capacity = 3;
  int args_amount = 0;

  char** args_array = (char**)malloc(sizeof(char*) * args_capacity);
  if (args_array == NULL){
    printf("ERROR: failed to malloc for args array\n");
    return NULL;
  }

  CursorState state = NORMAL;
  char* cursor = &input[0];

  int buff_capacity = 20;
  char* buffer = (char*)malloc(buff_capacity + 1);
  if (buffer == NULL){
    printf("ERROR: failed to malloc for args buffer");
    free(args_array);
    return NULL;
  }
  memset(buffer, '\0', buff_capacity + 1);
  int buff_idx = 0;

  while(state != END){
    if (state == NORMAL){
      parse_normal_state(&cursor, &state, delim);
    }
    else if (state == WRITING){
      parse_writing_state(&buffer, &state, &args_array,
                          &buff_idx, &args_amount, &args_capacity);
    }
    else if(state == READING){
      parse_reading_state(&cursor, &state, delim, &buffer, &buff_idx,
                          &buff_capacity, &args_array, args_amount);
    }
    else if (state == SQUOTE){
      parse_squote_state(&cursor, &state, &buffer, &buff_idx,
                          &buff_capacity, &args_array, args_amount);
    }
    else if(state == DQUOTE){
      parse_dquote_state(&cursor, &state, &buffer, &buff_idx,
                          &buff_capacity, &args_array, args_amount);
    }
  }

  args_array[args_amount] = NULL;
  free(buffer);

  return args_array;
}

void free_args(char** args){
  int idx = 0;
  while(args[idx] != NULL){
    free(args[idx]);
    idx++;
  }
  free(args);
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

