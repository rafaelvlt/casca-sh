#ifndef PARSING_H
#define PARSING_H

char** get_user_input();
char** parse_args(char* input);
void free_args(char** args);
char* search_program(const char* command)
;
#endif
