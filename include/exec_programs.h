#ifndef EXEC_PROGRAMS_H
#define EXEC_PROGRAMS_H

#include "redirection.h"
#include "autocomplete.h"

void handle_command(char* command, char** args);
void execute_external_program(char* command, char** args, info_redirection redirection);
char* search_program(const char* command);
void autocomplete_load_PATH(Trie *dictionary);

#endif
