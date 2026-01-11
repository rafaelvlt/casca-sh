#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "parsing.h"
#include "exec_programs.h"
#include "autocomplete.h"
#include "history.h"

int main(){
  setbuf(stdout, NULL);
  create_autocomplete_trie();
  init_history();

  bool repl_on = true;
  
  
  while (repl_on){
    char* cwd = getcwd(NULL, 0);
    char* prompt_cwd = process_prompt_cwd(cwd);
    char** args = get_user_input(prompt_cwd);
    free(prompt_cwd);  

    char* command = args[0];
    
    handle_command(command, args);
    free_args(args);
  }
  free_history();
  return EXIT_SUCCESS; 
}

