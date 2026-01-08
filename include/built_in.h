#ifndef BUILT_IN_H
#define BUILT_IN_H
#include "trie-dynamic.h"


typedef struct{
  void(* built_in_function)(char**);
  char* name;
} Built_In;

bool handle_built_in(char* command, char** args);

void autocomplete_load_built_in(Trie* dictionary);

void type_command(char** args);
void echo_command(char** args);
void pwd_command(char** args);
void cd_command(char** args);
void exit_command(char** args);

#endif
