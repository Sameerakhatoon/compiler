/*
* @file lexProcess.c
* @brief The lex process
* @details This file contains the lex process
*/

#include "compiler.h"
#include "stdlib.h"
#include "helpers/vector.h"

/*
* @fn LexProcess* create_lex_process(CompileProcess* compiler, LexProcessFunctions* functions, void* private_data)
* @brief Creates a lex process
* @details Creates and initializes a lexical analysis process with the given compiler, functions, and private data.
* @param compiler The compiler
* @param functions The functions
* @param private_data The private data
* @return The lex process
*/
LexProcess* create_lex_process(CompileProcess* compiler, LexProcessFunctions* functions, void* private_data){
    LexProcess* lex_process = calloc(1, sizeof(LexProcess));
    lex_process->functions = functions;
    lex_process->token_vector = create_vector(sizeof(Token));
    lex_process->compiler = compiler;
    lex_process->private_data = private_data;
    lex_process->position.file_name = compiler->input_file.absolute_path;
    lex_process->position.line = 1; 
    lex_process->position.column = 1;
    return lex_process;
}
/*
* @fn void free_lex_process(LexProcess* lex_process)
* @brief Frees a lex process
* @details Frees the allocated memory for the lexical analysis process.
* @param lex_process The lex process
* @return void
*/
void free_lex_process(LexProcess* lex_process){
    destroy_vector(lex_process->token_vector);
    free(lex_process);
}
/*
* @fn void* get_private_data_of_lex_process(LexProcess* lex_process)
* @brief Gets the private data of a lex process
* @details Retrieves the private data associated with the lexical process.
* @param lex_process The lex process
* @return The private data
*/
void* get_private_data_of_lex_process(LexProcess* lex_process){
    return lex_process->private_data;
}
/*
* @fn void set_private_data_of_lex_process(LexProcess* lex_process, void* private_data)
* @brief Sets the private data of a lex process
* @details Prints all tokens stored in the dynamic token vector.
* @param lex_process The lex process
* @param private_data The private data
* @return void
*/
void print_token_vector(DynamicVector* token_vector){
    printf("Size of token vector is: %i \n", get_element_count(token_vector));
    for(int i=0; i<get_element_count(token_vector); i++){
        print_token(get_element_at(token_vector, i));
    }
}
/*
* @fn DynamicVector* get_token_vector_of_lex_process(LexProcess* lex_process)
* @brief Gets the token vector of a lex process
* @details Retrieves the dynamic token vector associated with the lexical process.
* @param lex_process The lex process
* @return The token vector
*/
DynamicVector* get_token_vector_of_lex_process(LexProcess* lex_process){
    return lex_process->token_vector;
}