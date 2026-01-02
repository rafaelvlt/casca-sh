#ifndef PARSING_H
#define PARSING_H

char** get_user_input();
char** parse_args(char* input, char delim);
void free_args(char** args);
char* search_program(const char* command)
;
#endif
