#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 1024

typedef struct HistEntry{
  char* input;
} HistEntry;

typedef struct History{
  char* path;
  HistEntry history_array[HISTORY_SIZE];  
  unsigned long history_count;
} History;

void init_history();
void add_to_history(char* input);
void print_history(int last);
void sync_history();
void clear_history();
void free_history();

#endif

