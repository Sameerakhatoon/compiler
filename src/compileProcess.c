/*
* @file compilerProcess.c
* @brief The compiler process
* @details This file contains the implementation of the compiler process
*/

#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"
#include "helpers/vector.h"

/*
* @fn CompileProcess* create_compile_process(const char* in_file_name, const char* out_file_name, int flags)
* @brief Creates a compile process
* @details Initializes a compilation process by opening input/output files, allocating memory, and setting process flags.
* @param in_file_name The name of the input file
* @param out_file_name The name of the output file
* @param flags The flags for the compiler
* @return The result of the compilation
*/
CompileProcess* create_compile_process(const char* in_file_name, const char* out_file_name, int flags){
    printf("creating compile process\n");
    FILE* in_file = fopen(in_file_name, "r");
    printf("file opened\n");
    if(!in_file){
        printf("file not opened\n");
        // return (CompileProcess){.error = 1, .error_message = "Could not open input file"};
        return NULL;
    }

    FILE* out_file = NULL;
    if(out_file_name){
        printf("opening output file\n");
        out_file = fopen(out_file_name, "w");
        if(!out_file){
            printf("output file not opened\n");
            return NULL;
        }
    }

    CompileProcess* ptr_to_process = calloc(1, sizeof(CompileProcess));
    ptr_to_process->flags = flags;
    ptr_to_process->input_file.file_ptr = in_file;
    ptr_to_process->input_file.absolute_path = in_file_name;
    ptr_to_process->output_file = out_file;
    ptr_to_process->node_vector = create_vector(sizeof(Node* ));
    ptr_to_process->node_tree_vector = create_vector(sizeof(Node* ));
    return ptr_to_process;
}

/*
* @fn char compile_process_next_char(LexProcess* lex_process)
* @brief Gets the next character from the file
* @details Reads the next character from the input file, updating line and column tracking for accurate error reporting.
* @param lex_process The lex process
*/
char compile_process_next_char(LexProcess* lex_process){
    CompileProcess* compiler = lex_process->compiler;
    compiler->position.column++;
    char c = getc(compiler->input_file.file_ptr);
    if(c == '\n'){
        compiler->position.line++;
        compiler->position.column = 1;
    }
    return c;
}

/*
* @fn char compile_process_peek_char(LexProcess* lex_process)
* @brief Peeks the next character from the file
* @details Retrieves the next character from the input file without advancing the file pointer, preserving the current read position.
* @param lex_process The lex process
* @return The next character from the file
*/
char compile_process_peek_char(LexProcess* lex_process){
    CompileProcess* compiler = lex_process->compiler;
    char c = getc(compiler->input_file.file_ptr);
    ungetc(c, compiler->input_file.file_ptr);
    return c;
}
/*
* @fn void compile_process_push_char(LexProcess* lex_process, char c)
* @brief Pushes the next character back to the file
* @details Pushes a character back onto the input stream, effectively undoing the last read operation and adjusting the column position accordingly.
* @param lex_process The lex process
* @param c The next character
* @return void
*/
void compile_process_push_char(LexProcess* lex_process, char c){
    CompileProcess* compiler = lex_process->compiler;
    ungetc(c, compiler->input_file.file_ptr);
    compiler->position.column--;
}