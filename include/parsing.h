#ifndef PARSING_H
#define PARSING_H

typedef enum{
  NORMAL = 0,
  READING,
  WRITING,
  SQUOTE,
  DQUOTE,
  END
} CursorState;

char** get_user_input(char* prompt_cwd);
char** parse_args(char* input, char delim);
void free_args(char** args);
char* process_prompt_cwd(char* cwd);

#endif
