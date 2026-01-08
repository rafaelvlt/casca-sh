#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H
#include "trie-dynamic.h"

void create_autocomplete_trie();

char** autocomplete_user_input(const char* text, int start, int end);

char* autocomplete_generator(const char* text, int state);

char** get_matches_array(Trie* dict, const char* prefix, int* count);

void autocomplete_dfs(Trie* node, char* text, int depth,
                      char*** matches_ptr, int* count, int* capacity);

#endif

