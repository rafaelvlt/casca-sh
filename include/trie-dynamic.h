#ifndef TRIE_H
#define TRIE_H

typedef struct Trie {
    char letter;
    bool is_leaf;
    int children_capacity;
    int children_quantity;
    struct Trie** children;
} Trie;

Trie* trieCreate();

void trieInsert(Trie* obj, char* word);

bool trieSearch(Trie* obj, char* word);

bool trieStartsWith(Trie* obj, char* prefix);
    
void trieFree(Trie* obj);

#endif
