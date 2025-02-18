/*
* @file compiler.h
* @brief The main header file for the compiler
* @details This file contains the declarations for the compiler, lexer, tokens, and other helper functions
*/

#ifndef COMPILER_H
#define COMPILER_H

#include "stdio.h"
#include "stdbool.h"
#include "helpers/vector.h"
#include "helpers/buffer.h"
#include "string.h"


//declarations for compiler beign here

/*
* @enum
* @brief Flags for the compiler
*/
enum{
    COMPILER_SUCCESS = 0,
    COMPILER_FAILED_WITH_ERRORS = 1
};
/*
* @struct PositionInFile
* @brief Position of a token in a file or character that's been read for lexing
* @details This struct contains the line number, column number, and the file name of the file being read
* @var PositionInFile::line
* Member 'line' contains the line number of the token
* @var PositionInFile::column
* Member 'column' contains the column number of the token
* @var PositionInFile::file_name
* Member 'file_name' contains the name of the file being read
*/
typedef struct PositionInFile PositionInFile;
struct PositionInFile{
    unsigned int line;
    unsigned int column;
    const char* file_name;
};

typedef struct Scope {
    int flags;
    DynamicVector* entities;
    size_t size; //total number of bytes scope is using, aligned to 16 bytes, so stack is aligned to 16 bytes
    struct Scope* parent;
} Scope;

typedef struct Symbol {
    const char* name;
    int type;
    void* data;
} Symbol;
enum{
    SYMBOL_TYPE_NODE,
    SYMBOL_TYPE_NATIVE_FUNCTION,
    SYMBOL_TYPE_UNKNOWN
};

/*
* @struct CompileProcess
* @brief The process of compiling a file
* @details This struct contains the flags for the compiler, the position in the file, the input file, the output file, and the tokens from the lexer
* @var CompileProcess::flags
* Member 'flags' contains the flags for the compiler
* @var CompileProcess::position
* Member 'position' contains the position in the file
* @var CompileProcess::input_file
* Member 'input_file' contains the input file
* @var CompileProcess::input_file::file_ptr
* Member 'file_ptr' contains the pointer to the input file
* @var CompileProcess::input_file::absolute_path
* Member 'absolute_path' contains the absolute path of the input file
* @var CompileProcess::output_file
* Member 'output_file' contains the pointer to the output file
* @var CompileProcess::token_vector
* Member 'token_vector' contains the tokens from the lexer
* @var CompileProcess::node_vector
* Member 'node_vector' contains the nodes for push & pop operations for the parser
* @var CompileProcess::node_tree_vector
* Member 'node_tree_vector' contains the root of the parse tree
*/
typedef struct CompileProcess CompileProcess;
struct CompileProcess{
    int flags; // the flags in regards to how this file should be compiled
    PositionInFile position;
    struct CompileProcessInputFile{
        FILE* file_ptr;
        const char* absolute_path;
    } input_file;
    DynamicVector* token_vector; //tokens from the lexer
    DynamicVector* node_vector; //nodes for push & pop operations for the parser
    DynamicVector* node_tree_vector; //root of parse tree

    FILE* output_file;
    struct{
        Scope* root;
        Scope* current;
    } scope;

    struct{
        DynamicVector* current_active_symbol_table; //holds struct sybmol pointers
        DynamicVector* tables; //holds struct dynamic vectors, each dynamic vector holds struct symbol pointers
    } symbols;
};
/*
* @fn int compile_file(const char* in_file_name, const char* out_file_name, int flags)
* @brief Compiles a file
* @details This function compiles a file and returns the result
* @param in_file_name The name of the input file
* @param out_file_name The name of the output file
* @param flags The flags for the compiler
* @return The result of the compilation
*/
int compile_file(const char* in_file_name, const char* out_file_name, int flags);
/*
* @fn CompileProcess* create_compile_process(const char* in_file_name, const char* out_file_name, int flags)
* @brief Creates a compile process
* @details This function creates a compile process and returns the result
* @param in_file_name The name of the input file
* @param out_file_name The name of the output file
* @param flags The flags for the compiler
* @return The result of the compilation
*/
CompileProcess* create_compile_process(const char* in_file_name, const char* out_file_name, int flags);

//declarations for tokens begin here

typedef struct Token Token;

/*
* @enum
* @brief The type of the token
* @details This enum contains the types of the token
* @var NUMBER_TYPE_NORMAL_INT
* Member 'NUMBER_TYPE_NORMAL_INT' represents a normal integer
* @var NUMBER_TYPE_LONG
* Member 'NUMBER_TYPE_LONG' represents a long integer
* @var NUMBER_TYPE_FLOAT
* Member 'NUMBER_TYPE_FLOAT' represents a float
* @var NUMBER_TYPE_DOUBLE
* Member 'NUMBER_TYPE_DOUBLE' represents a double
*/
enum{
    NUMBER_TYPE_NORMAL_INT,
    NUMBER_TYPE_LONG,
    NUMBER_TYPE_FLOAT,
    NUMBER_TYPE_DOUBLE,
};
/*
* @struct Token
* @brief The token
* @details This struct contains the value, type, flags, position, number, is_whitespace, and whats_between_brackets of the token
* @var Token::value
* Member 'value' contains the value of the token
* @var Token::value::char_val
* Member 'char_val' contains the character value of the token
* @var Token::value::string_val
* Member 'string_val' contains the string value of the token
* @var Token::value::int_num
* Member 'int_num' contains the integer value of the token
* @var Token::value::long_num
* Member 'long_num' contains the long integer value of the token
* @var Token::value::long_long_num
* Member 'long_long_num' contains the long long integer value of the token
* @var Token::value::any_ptr
* Member 'any_ptr' contains the pointer value of the token
* @var Token::type
* Member 'type' contains the type of the token
* @var Token::flags
* Member 'flags' contains the flags of the token
* @var Token::position
* Member 'position' contains the position of the token
* @var Token::Number
* Member 'Number' contains the number of the token
* @var Token::Number::type
* Member 'type' contains the type of the number
* @var Token::is_whitespace
* Member 'is_whitespace' contains the whitespace of the token
* @var Token::whats_between_brackets
* Member 'whats_between_brackets' contains the string between the brackets of the token
*/
struct Token{
    union{
        char char_val;
        const char* string_val;
        unsigned int int_num;
        unsigned long long_num;
        unsigned long long long_long_num;
        void* any_ptr;
    } value;
    int type;
    int flags;
    struct PositionInFile position;

    struct TokenNumber{
        int type;
    } Number;

    bool is_whitespace; // true if there's a whitespace character between this token and previous token, eg: * a for operator token * would mean there's a whitespace between * and a, and is_whitespace would be true for the token "a"

    const char* whats_between_brackets; // if this token is a bracket, this will be the string between the brackets, eg: (hello), whats_between_brackets would point to start of "hello" and the token would be a bracket token??


};
/*
* @enum
* @brief The type of the token
* @details This enum contains the types of the token
* @var TOKEN_TYPE_IDENTIFIER
* Member 'TOKEN_TYPE_IDENTIFIER' represents an identifier
* @var TOKEN_TYPE_KEYWORD
* Member 'TOKEN_TYPE_KEYWORD' represents a keyword
* @var TOKEN_TYPE_OPERATOR
* Member 'TOKEN_TYPE_OPERATOR' represents an operator
* @var TOKEN_TYPE_SYMBOL
* Member 'TOKEN_TYPE_SYMBOL' represents a symbol
* @var TOKEN_TYPE_STRING
* Member 'TOKEN_TYPE_STRING' represents a string
* @var TOKEN_TYPE_COMMENT
* Member 'TOKEN_TYPE_COMMENT' represents a comment
* @var TOKEN_TYPE_NEWLINE
* Member 'TOKEN_TYPE_NEWLINE' represents a newline
* @var TOKEN_TYPE_NUMBER
* Member 'TOKEN_TYPE_NUMBER' represents a number
*/
enum{
    
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_SYMBOL,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NEWLINE,
    TOKEN_TYPE_NUMBER,
    // TOKEN_TYPE_WHITESPACE,

    // TOKEN_TYPE_EOF
    //     TOKEN_TYPE_LITERAL,
    // TOKEN_TYPE_BRACKET,
};

//declarations for lexer begin here

/*
* @struct LexProcess
* @brief The process of lexing a file
* @details This struct contains the position, token vector, compiler, current expression count, parenthesis buffer, functions, and private data of the lex process
* @var LexProcess::position
* Member 'position' contains the position of the lex process
* @var LexProcess::token_vector
* Member 'token_vector' contains the token vector of the lex process
* @var LexProcess::compiler
* Member 'compiler' contains the compiler of the lex process
* @var LexProcess::current_expression_count
* Member 'current_expression_count' contains the current expression count of the lex process
* @var LexProcess::parenthesis_buffer
* Member 'parenthesis_buffer' contains the parenthesis buffer of the lex process
* @var LexProcess::functions
* Member 'functions' contains the functions of the lex process
* @var LexProcess::private_data
* Member 'private_data' contains the private data of the lex process
*/
typedef struct LexProcess LexProcess;
/* 
* @struct LexProcessFunctions
* @brief The functions of the lex process
* @details This struct contains the next char, peek char, and push char functions of the lex process
* @var LexProcessFunctions::next_char
* Member 'next_char' contains the next char function of the lex process
* @var LexProcessFunctions::peek_char
* Member 'peek_char' contains the peek char function of the lex process
* @var LexProcessFunctions::push_char
* Member 'push_char' contains the push char function of the lex process
*/
typedef struct LexProcessFunctions LexProcessFunctions ; //functions that the lexer will use to read the file

/*
* @typedef LEX_PROCESS_NEXT_CHAR
* @brief The next char function of the lex process
* @details This function gets the next character from the file, peek the next character, and push the next character
*/
typedef char (*LEX_PROCESS_NEXT_CHAR)(LexProcess* lex_process); //function to get the next character from the file, peek the next character, and push the next character
/*
* @typedef LEX_PROCESS_PEEK_CHAR
* @brief The peek char function of the lex process
* @details This function peeks the next character from the file
*/
typedef char (*LEX_PROCESS_PEEK_CHAR)(LexProcess* lex_process); //function to peek the next character from the file
/*
* @typedef LEX_PROCESS_PUSH_CHAR
* @brief The push char function of the lex process
* @details This function pushes the next character back to the file
*/
typedef void (*LEX_PROCESS_PUSH_CHAR)(LexProcess* lex_process, char character); //function to push a character back to the file
struct LexProcessFunctions {
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};
struct LexProcess{
    PositionInFile position;
    DynamicVector* token_vector;
    CompileProcess* compiler;

    int current_expression_count; // the number of expressions that have been read so far, that's number of brackets that have been opened and not closed yet
    BufferType* parenthesis_buffer; // buffer to store the string between brackets
    LexProcessFunctions* functions;

    void* private_data; // private data that the lexer don't understand but the one using lexer does
};
/*
* @fn char compile_process_next_char(LexProcess* lex_process)
* @brief Gets the next character from the file
* @details This function gets the next character from the file
* @param lex_process The lex process
* @return The next character from the file
*/
char compile_process_next_char(LexProcess* lex_process);
/*
* @fn char compile_process_peek_char(LexProcess* lex_process)
* @brief Peeks the next character from the file
* @details This function peeks the next character from the file
* @param lex_process The lex process
* @return The next character from the file
*/
char compile_process_peek_char(LexProcess* lex_process);
/*
* @fn void compile_process_push_char(LexProcess* lex_process, char c)
* @brief Pushes the next character back to the file
* @details This function pushes the next character back to the file
* @param lex_process The lex process
* @param c The next character
* @return void
*/
void compile_process_push_char(LexProcess* lex_process, char c);
/*
* @fn LexProcess* create_lex_process(CompileProcess* compiler, LexProcessFunctions* functions, void* private_data)
* @brief Creates a lex process
* @details This function creates a lex process and returns the result
* @param compiler The compiler
* @param functions The functions of the lex process
* @param private_data The private data of the lex process
* @return The result of the lex process
* @note The private data is the data that the lexer doesn't understand but the one using lexer does
*/
LexProcess* create_lex_process(CompileProcess* compiler, LexProcessFunctions * functions, void* private_data);
/*
* @fn void free_lex_process(LexProcess* lex_process)
* @brief Frees the lex process
* @details This function frees the lex process
* @param lex_process The lex process
* @return void
*/
void free_lex_process(LexProcess* lex_process);
/*
* @fn void* get_private_data_of_lex_process(LexProcess* lex_process)
* @brief Gets the private data of the lex process
* @details This function gets the private data of the lex process
* @param lex_process The lex process
* @return The private data of the lex process
*/
void* get_private_data_of_lex_process(LexProcess* lex_process);
/*
* @fn DynamicVector* get_token_vector_of_lex_process(LexProcess* lex_process)
* @brief Gets the token vector of the lex process
* @details This function gets the token vector of the lex process
* @param lex_process The lex process
* @return The token vector of the lex process
*/
DynamicVector* get_token_vector_of_lex_process(LexProcess* lex_process);
/*
* @fn int lex(LexProcess* lex_process)
* @brief Lexes a file
* @details This function lexes a file and returns the result
* @param lex_process The lex process
* @return The result of the lex process
*/
int lex(LexProcess* lex_process);
/*
* @enum
* @brief The result of the lexical analysis
* @details This enum contains the results of the lexical analysis
* @var LEXICAL_ANALYSIS_SUCCESS
* Member 'LEXICAL_ANALYSIS_SUCCESS' represents a successful lexical analysis
* @var LEXICAL_ANALYSIS_FAILED_WITH_ERRORS
* Member 'LEXICAL_ANALYSIS_FAILED_WITH_ERRORS' represents a failed lexical analysis with errors
*/
enum{
    LEXICAL_ANALYSIS_SUCCESS,
    LEXICAL_ANALYSIS_FAILED_WITH_ERRORS
};
/*
* @fn compile_error(CompileProcess* process, const char* msg, ...)
* @brief Prints an error message
* @details This function prints an error message
* @param process The compile process
* @param msg The message
* @return void
*/
void compiler_error(CompileProcess* process, const char* msg, ...);
/*
* @fn compile_warning(CompileProcess* process, const char* msg, ...)
* @brief Prints a warning message
* @details This function prints a warning message
* @param process The compile process
* @param msg The message
* @return void
*/
void compiler_warning(CompileProcess* process, const char* msg, ...);
/*
* @typedef NUMERIC_CASES
* @brief The numeric cases
* @details This function contains the numeric cases
*/
#define NUMERIC_CASES \
    case '0': \
    case '1': \
    case '2': \
    case '3': \
    case '4': \
    case '5': \
    case '6': \
    case '7': \
    case '8': \
    case '9'
/*
* @fn void print_token(Token* token)
* @brief Prints a token
* @details This function prints a token
* @param token The token
* @return void
*/
void print_token_vector(DynamicVector* token_vector);
/*
* @fn void print_token(Token* token)
* @brief Prints a token
* @details This function prints a token
* @param token The token
* @return void
*/
void print_token(Token* token);
/*
* @typedef OPERATOR_CASE_EXCLUDING_DIVISION
* @brief The operator case excluding division
* @details This function contains the operator case excluding division
*/
#define OPERATOR_CASE_EXCLUDING_DIVISION \
    case '+': \
    case '-': \
    case '*': \
    case '>': \
    case '<': \
    case '%': \
    case '=': \
    case '?': \
    case '!': \
    case '&': \
    case '|': \
    case '^': \
    case '~': \
    case '.': \
    case ',': \
    case '(': \
    case '['
/*
* @typedef ARE_STRINGS_EQUAL(string1, string2)
* @brief Compares two strings
*/
#define ARE_STRINGS_EQUAL(string1, string2) \
    (string1 && string2 && (strcmp(string1, string2) == 0))
/*
* @fn bool is_token_keyword(Token* token, const char* value)
* @brief Checks if a token is a keyword
* @details This function checks if a token is a keyword
*/
bool is_token_keyword(Token* token, const char* value);
// - Symbol: {, },:,;, #, \,),]
/*
* @typedef SYMBOL_CASE
* @brief The symbol case
* @details This function contains the symbol case
*/
#define SYMBOL_CASE \
    case '{': \
    case '}': \
    case ':': \
    case ';': \
    case '#': \
    case ')': \
    case ']': \
    case '\\'

//to create tokens outside lexer
/*
* @fn LexProcess* build_tokens_for_string(CompileProcess* compiler, const char* string)
* @brief Builds tokens for a string
* @details This function builds tokens for a string
*/
LexProcess* build_tokens_for_string(CompileProcess* compiler, const char* string); //equivalent to yy_scan_string in flex

/*
In Flex, yylex() returns an int representing the token type, but in your lexer, it should return a Token*.
*/
//yylex() Equivalent, it gets the next token
Token* yylex();
/*
yytext is supposed to hold the last matched token’s text.
*/
//yytext equivalent, it gets the string value of the token
const char* yytext();
/*
yyleng should return the length of the last matched text (yytext).
*/
//yyleng equivalent, it gets the length of the string value of the token
int yyleng();

typedef struct Node Node;
enum{
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESES,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STRING,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_LIST,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BODY,
    NODE_TYPE_STATEMENT_RETURN,
    NODE_TYPE_STATEMENT_IF,
    NODE_TYPE_STATEMENT_ELSE,
    NODE_TYPE_STATEMENT_ELSE_IF,
    NODE_TYPE_STATEMENT_WHILE,
    NODE_TYPE_STATEMENT_DO_WHILE,
    NODE_TYPE_STATEMENT_FOR,
    NODE_TYPE_STATEMENT_BREAK,
    NODE_TYPE_STATEMENT_CONTINUE,
    NODE_TYPE_STATEMENT_SWITCH,
    NODE_TYPE_STATEMENT_CASE,
    NODE_TYPE_STATEMENT_DEFAULT,
    NODE_TYPE_STATEMENT_GOTO,

    NODE_TYPE_UNARY,
    NODE_TYPE_TERNARY,
    NODE_TYPE_LABEL,
    NODE_TYPE_STRUCT,
    NODE_TYPE_UNION,
    NODE_TYPE_BRACKET,
    NODE_TYPE_CAST,
    NODE_TYPE_BLANK,
};
typedef struct ArrayBrackets{
    DynamicVector* n_brackets;
}ArrayBrackets;
typedef struct DataType{
    int flags;
    int type;
    struct DataType* secondary_data_type; //example for long long, long is the secondary data type. for long int, int is the secondary data type
    const char* name;
    size_t size;
    int pointer_level; //0 means no pointer, 1 means *, 2 means **, etc
    union{
        Node* struct_node;
        Node* union_node;
    } data;
    struct{
        struct ArrayBrackets* array_bracket;
        size_t size; //size of complete array, datatype_size*number_of_elements_in_array = datatype_size*array_index*array_index
    } array;
}DataType;

struct Node{
    union{
        char char_val;
        const char* string_val;
        unsigned int int_num;
        unsigned long long_num;
        unsigned long long long_long_num;
    } literal_value;
    union{
        struct expression{
            Node* left;
            Node* right;
            const char* operator;
        } expression;
        struct var{
            struct DataType data_type;
            const char* name;
            Node* value;
            int padding;
            int aligned_offset;
        } var;
        struct variable_list{
            DynamicVector* variables;
        } variable_list;
        struct bracket{
            Node* inner;
        } bracket;
        struct structure{
            const char* name;
            Node* body_node;
            Node* variable;
        } structure;
        struct body{
            DynamicVector* statements;
            size_t size;
            bool padded;
            Node* largest_var_node;
        }body;
    } data;
    int type;
    int flags;
    PositionInFile position;
    struct BindedTo{
        //pointer to body node the node is in
        Node* body;
        //pointer to function the node is in
        Node* function;
    } BindedTo;
}; 

enum{
    PARSER_SUCCESS,
    PARSER_FAILED_WITH_ERRORS
};

int parse(CompileProcess* compiler);

bool is_token_keyword(Token* token, const char* value);
bool parser_ignore_nl_or_comment_or_nl_seperator_tokens(Token* token);
bool is_token_symbol(Token* token, char symbol);

void set_node_vectors(DynamicVector* vector, DynamicVector* root);

void push_node(Node* node);

Node* peek_node_or_null();

Node* peek_node();

Node* pop_node();

void print_node(Node* node, int depth);

void print_node_vector(DynamicVector* node_vector);

enum{
    NODE_FLAG_INSIDE_EXPRESSION = 0b00000001,
};

Node* peek_node_expressionable_or_null();

void make_expression_node(Node* left, Node* right, const char* operator);

Node* create_node(Node* node);

#define TOTAL_OPERATOR_GROUPS 14
#define MAX_OPERATIONS_IN_GROUP 12

typedef enum{
    ASSOCIATIVITY_LEFT_TO_RIGHT,
    ASSOCIATIVITY_RIGHT_TO_LEFT
} associativity_types;

typedef struct ExpressionableOperatorPrecedanceGroup{
    char* operators[MAX_OPERATIONS_IN_GROUP];
    int associativity;
} ExpressionableOperatorPrecedanceGroup;

bool keyword_is_datatype(const char* value);

enum{
    DATATYPE_FLAG_IS_SIGNED = 0b00000001,
    DATATYPE_FLAG_IS_STATIC = 0b00000010,
    DATATYPE_FLAG_IS_CONST = 0b00000100,
    DATATYPE_FLAG_IS_POINTER = 0b00001000,
    DATATYPE_FLAG_IS_ARRAY = 0b00010000,
    DATATYPE_FLAG_IS_EXTERN = 0b00100000,
    DATATYPE_FLAG_IS_RESTRICT = 0b01000000,
    DATATYPE_FLAG_IGNORE_TYPE_CHECK = 0b10000000,
    DATATYPE_FLAG_IS_SECONDARY = 0b100000000,
    DATATYPE_FLAG_STRUCT_OR_UNION_NO_NAME = 0b1000000000,
    DATATYPE_FLAG_IS_LITERAL = 0b10000000000,
};

enum{
    DATA_TYPE_VOID,
    DATA_TYPE_INT,
    DATA_TYPE_CHAR,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_LONG,
    DATA_TYPE_SHORT,
    DATA_TYPE_STRUCT,
    DATA_TYPE_UNION,
    DATA_TYPE_UNKNOWN,
};

bool is_token_primitive_keyword(Token* token);

enum{
    DATA_TYPE_EXPECT_PRIMITIVE,
    DATA_TYPE_EXPECT_STRUCT,
    DATA_TYPE_EXPECT_UNION,    
};

bool is_datatype_struct_or_union_given_name(const char* name);

enum{
    DATA_SIZE_ZERO = 0,
    DATA_SIZE_BYTE = 1,
    DATA_SIZE_WORD = 2,
    DATA_SIZE_DWORD = 4,
    DATA_SIZE_DDWORD = 8,
};

ArrayBrackets* array_brackets_new(int size);

void free_array_brackets(ArrayBrackets* array_brackets);

void add_array_bracket(ArrayBrackets* array_brackets, Node* bracket_node);

DynamicVector* get_array_brackets_node_vector(ArrayBrackets* array_brackets);

size_t array_brackets_calculate_size_from_index(DataType* datatype, ArrayBrackets* array_brackets, int index);

size_t array_brackets_calculate_size(DataType* datatype, ArrayBrackets* array_brackets);

int array_brackets_get_total_indices(DataType* datatype);

bool is_datatype_struct_or_union(DataType* datatype);

bool is_datatype_struct_or_union_given_name(const char* name);

Scope* allocate_scope();

Scope* create_root_scope(CompileProcess* process);

Scope* deallocate_scope(Scope* scope);

void free_root_scope(CompileProcess* process);

Scope* new_scope(CompileProcess* process, int flags);

void scope_iteration_start(Scope* scope);

void scope_iteration_end(Scope* scope);

void* scope_iterate_back(Scope* scope);

void* get_last_entity_of_scope(Scope* scope);

void* get_last_entity_from_scope_stop_at(Scope* scope, Scope* stop_scope);

void* get_scope_last_entity_stop_at(CompileProcess *process, Scope* stop_scope);

void* get_scope_last_entity(CompileProcess* process);

void push_scope(CompileProcess* process, void* pointer, size_t element_size);

void finish_scope(CompileProcess* process);

Scope* get_current_scope(CompileProcess* process);


size_t get_datatype_size(DataType* datatype);
size_t get_datatype_size_no_pointer(DataType* datatype);

size_t get_datatype_size_for_array_access(DataType* datatype);
size_t get_datatype_element_size(DataType* datatype);

size_t get_variable_size(Node* variable_node);

size_t get_variable_size_for_list(Node* variable_list_node);


int get_padding(int value, int to);

int get_align_value(int val, int to);
int get_align_value_treat_positive(int val, int to);
int get_compute_sum_padding(DynamicVector* vector);

bool is_node_struct_or_union_variable(Node* node);

Node* get_variable_struct_or_union_body_node(Node* node);

Node* get_variable_node(Node* node);

bool is_variable_node_primitive(Node* node);

bool is_datatype_primitive(DataType* datatype);

Node* get_variable_node_or_list(Node* node);

#endif