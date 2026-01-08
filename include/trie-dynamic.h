#ifndef TRIE_H
#define TRIE_H

typedef struct Trie {
    char letter;
    bool is_leaf;
    int children_capacity;
    int children_quantity;
    struct Trie** children;
} Trie;

Trie* trie_create();

void trie_insert(Trie* obj, char* word);

bool trie_search(Trie* obj, const char* word);

bool trie_starts_with(Trie* obj, const char* prefix);
    
void trie_free(Trie* obj);

#endif
