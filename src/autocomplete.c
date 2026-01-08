#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "built_in.h"
#include "trie-dynamic.h"
#include "autocomplete.h"

static Trie* autocomplete_dictionary = NULL;

void create_autocomplete_trie(){
  autocomplete_dictionary = trie_create();
  autocomplete_load_built_in(autocomplete_dictionary);
  rl_attempted_completion_function = autocomplete_user_input;
}

char** autocomplete_user_input(const char* text, int start, [[maybe_unused]] int end){
  if (start == 0){
    return rl_completion_matches(text, autocomplete_generator);
  } 

  return NULL;
}

char* autocomplete_generator(const char* text, int state){
  static char** matches = NULL;
  static int count = 0;
  if (trie_starts_with(autocomplete_dictionary, text)){
    if (state == 0){
      count = 0;
      if (matches != NULL){
        //readline already free's the strings after using, only need to free this
        free(matches);
        matches = NULL;
      }
      matches = get_matches_array(autocomplete_dictionary, text, &count);
    }
    
    if (matches != NULL && state < count){
      return matches[state];
    }
  }

  return NULL;
}

char** get_matches_array(Trie* dict, const char* prefix, int* count){
  Trie* prefix_start = dict;
  int i = 0;
  while (*(prefix + i) != '\0'){
    bool found = false;
    for (int j = 0; !found && j < prefix_start->children_quantity; j++){
      if (prefix_start->children[j]->letter == *(prefix + i)){
        prefix_start = prefix_start->children[j];
        found = true;
      }
    }
    if (found) i++;
    else return NULL;
  }

  int capacity = 3;
  char text[256];
  strncpy(text, prefix, 256);
  char** matches = (char**)malloc(sizeof(char*) * capacity);
  autocomplete_dfs(prefix_start, text, strlen(prefix), &matches, count, &capacity);
  
  return matches;
}

void autocomplete_dfs(Trie* node, char* text, int depth,
                      char*** matches_ptr, int* count, int* capacity)
{
  if (node->is_leaf){
    if (*count + 1 == *capacity){
      *capacity *= 2;
      char** temp = realloc((*matches_ptr), sizeof(char*) * *capacity);
      if (temp == NULL){
        printf("FATAL ERROR: failed to reallocate (*matches_ptr) array in dfs. exiting");
        exit(1);
      }
      (*matches_ptr) = temp;
    }       
    (*matches_ptr)[(*count)++] = strdup(text);
  }
  for (int i = 0; i < node->children_quantity; i++){
    text[depth] = node->children[i]->letter;
    text[depth+1] = '\0';
    autocomplete_dfs(node->children[i], text, depth+1, matches_ptr, count, capacity);
  }
}
