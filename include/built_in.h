#ifndef BUILT_IN_H
#define BUILT_IN_H

#include "redirection.h"
typedef struct{
  void(* built_in_function)(char**);
  const char* name;
} Built_In;

bool handle_built_in(char* command, char** args);
void type_command(char** args);
void echo_command(char** args);
void pwd_command(char** args);
void cd_command(char** args);
void exit_command(char** args);

#endif
