#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trie-dynamic.h"

Trie* trie_create() {
    Trie* node = (Trie*)malloc(sizeof(Trie));

    int capacity = 1;
    Trie** children = (Trie**)calloc(capacity, sizeof(Trie*));
    
    node->children = children;
    node->children_capacity = capacity;
    node->children_quantity = 0;
    node->letter = '\0';
    node->is_leaf = false;
    return node;
}

void trie_insert(Trie* obj, char* word) {
  if (*word == '\0'){
    obj->is_leaf = true;
    return;
  }

  bool found = false;
  Trie* target = NULL;
  for (int i = 0; !found && i < obj->children_quantity; i++){
    if (*word == obj->children[i]->letter){
      found = true;
      target = obj->children[i];
    }
  }
  if (!found){
    if (obj->children_quantity + 1 >= obj->children_capacity){
      obj->children_capacity *= 2;
      Trie** temp = realloc(obj->children, sizeof(Trie*) * obj->children_capacity);
      if (temp == NULL){
        printf("FATAL ERROR: Failed to reallocate for Trie, exiting.");
        exit(1);
      }
      obj->children = temp;
    }
    obj->children[obj->children_quantity] = trie_create();
    obj->children[obj->children_quantity]->letter = *word;
    trie_insert(obj->children[obj->children_quantity], word+1);
    obj->children_quantity++;
  }
  else{
    trie_insert(target, word+1);
  }
}

bool trie_search(Trie* obj, const char* word) {
  if (*word == '\0'){
    if (obj->is_leaf){
      return true;
    }
    else{
      return false;
    }
  }
  for (int i = 0; i < obj->children_quantity; i++){
    if (obj->children[i]->letter == *word){
      return trie_search(obj->children[i], word+1);
    }
  }
  return false;
}

bool trie_starts_with(Trie* obj, const char* prefix) {
  if (*prefix == '\0'){
      return true;
  }
  for (int i = 0; i < obj->children_quantity; i++){
    if (obj->children[i]->letter == *prefix){
      return trie_starts_with(obj->children[i], prefix+1);
    }
  }
  return false;
}

void trie_free(Trie* obj) {
  if (obj == NULL) return;

  for (int i = 0; i < obj->children_quantity; i++){
    trie_free(obj->children[i]);
  }
  free(obj->children);
  free(obj);
}
