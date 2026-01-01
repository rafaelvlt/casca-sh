#include <stdio.h>
#include <stdlib.h>
#include "parsing.h"
#include "exec_programs.h"

int main(){
  setbuf(stdout, NULL);

  bool repl_on = true;

  while (repl_on){
    printf("$ ");

    char** args = get_user_input();
    char* command = args[0];
    
    handle_command(command, args);
    free_args(args);
  }

  return EXIT_SUCCESS; 
}
