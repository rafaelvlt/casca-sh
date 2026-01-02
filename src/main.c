#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parsing.h"
#include "exec_programs.h"

int main(){
  setbuf(stdout, NULL);

  bool repl_on = true;
  
  while (repl_on){
    char* cwd = getcwd(NULL, 0);
    char* prompt_cwd = process_prompt_cwd(cwd);
    printf("⭑%s⭑-» ", prompt_cwd);
    free(prompt_cwd);  

    char** args = get_user_input();
    char* command = args[0];
    
    handle_command(command, args);
    free_args(args);
  }

  return EXIT_SUCCESS; 
}

