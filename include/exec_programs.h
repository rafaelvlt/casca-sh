#ifndef EXEC_PROGRAMS_H
#define EXEC_PROGRAMS_H

void handle_command(char* command, char** args);
void execute_external_program(char* command, char** args);
char* search_program(const char* command);

#endif
