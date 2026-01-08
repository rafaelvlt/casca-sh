#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trie.h"

Trie* trieCreate() {
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

void trieInsert(Trie* obj, char* word) {
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
    obj->children[obj->children_quantity] = trieCreate();
    obj->children[obj->children_quantity]->letter = *word;
    trieInsert(obj->children[obj->children_quantity], word+1);
    obj->children_quantity++;
  }
  else{
    trieInsert(target, word+1);
  }
}

bool trieSearch(Trie* obj, char* word) {
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
      return trieSearch(obj->children[i], word+1);
    }
  }
  return false;
}

bool trieStartsWith(Trie* obj, char* prefix) {
  if (*prefix == '\0'){
      return true;
  }
  for (int i = 0; i < obj->children_quantity; i++){
    if (obj->children[i]->letter == *prefix){
      return trieStartsWith(obj->children[i], prefix+1);
    }
  }
  return false;
}

void trieFree(Trie* obj) {
  if (obj == NULL) return;

  for (int i = 0; i < obj->children_quantity; i++){
    trieFree(obj->children[i]);
  }
  free(obj->children);
  free(obj);
}
