/*
* @file compiler.c
* @brief The compiler
* @details This file contains the implementation of the compiler
*/

#include "compiler.h"
#include <stdarg.h>
#include <stdlib.h>

//set up the functions for the lexer
LexProcessFunctions lex_functions = {
    .next_char = compile_process_next_char,
    .peek_char = compile_process_peek_char,
    .push_char = compile_process_push_char
};

/*
* @fn int compile_file(const char* in_file_name, const char* out_file_name, int flags)
* @brief Compiles a file
* @details Compiles the given source file by creating a compile process, performing lexical analysis, storing tokens, and preparing for parsing and code generation, returning success or failure status.
* @param in_file_name The name of the input file
* @param out_file_name The name of the output file
* @param flags The flags for the compiler
* @return The result of the compilation
*/
int compile_file(const char* in_file_name, const char* out_file_name, int flags){
    printf("compiling file %s\n", in_file_name);
    CompileProcess* process = create_compile_process(in_file_name, out_file_name, flags);
    if(!process){
        return COMPILER_FAILED_WITH_ERRORS;
    }
    //perfoem lexical analysis
    LexProcess* lex_process = create_lex_process(process, &lex_functions, NULL);
    if(!lex_process){
        return COMPILER_FAILED_WITH_ERRORS;
    }
    if(lex(lex_process)!=LEXICAL_ANALYSIS_SUCCESS){
        return COMPILER_FAILED_WITH_ERRORS;
    }
    process->token_vector = lex_process->token_vector;
    print_token_vector(process->token_vector);

    //perform parsing
    if(parse(process)!=PARSER_SUCCESS){
        return COMPILER_FAILED_WITH_ERRORS;
    }
    print_node_vector(process->node_vector);
    print_node_vector(process->node_tree_vector);
    //perfoem code generation
    
    return COMPILER_SUCCESS;
}

/*
* @fn void compiler_error(CompileProcess* process, const char* msg, ...)
* @brief Prints an error message
* @details Reports a compilation error with a formatted message, including the line, column, and file name, then terminates the process.
*/
void compiler_error(CompileProcess* process, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, " on line %i, column %i in file %s\n", process->position.line, process->position.column, process->input_file.absolute_path);
    exit(-1);
}

/*
* @fn void compiler_warning(CompileProcess* process, const char* msg, ...)
* @brief Prints a warning message
* @details Displays a formatted compiler warning message with line, column, and file details but does not terminate execution.
*/
void compiler_warning(CompileProcess* process, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, " on line %i, column %i in file %s\n", process->position.line, process->position.column, process->input_file.absolute_path);
}