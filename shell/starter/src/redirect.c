#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include "redirect.h"

void handle_redirection(tokenlist *tokens){
    // storage for i/o files
    char *input_file = NULL;
    char *output_file = NULL;
    
    // loop through tokens for < or >
    for(int i = 0; i < tokens->size; i++){
        char *token = tokens->items[i];
        
        // cmd < file_in
        if(strcmp(token,"<") == 0){
            if(i + 1 < tokens->size){
                input_file = tokens->items[i+1];
            }
        }

        // cmd > file_out
        else if(strcmp(token,">") == 0){
            if(i + 1 < tokens->size){
                output_file = tokens->items[i+1];
            }
        }
    }

    if(input_file != NULL){
        // open input file, dup2, close
    }
    if(output_file != NULL){
        // open output file, dup2, close
    }
}