/*
* @file lexer.c
* @brief The lexer
* @details This file contains the lexer
*/

#include "compiler.h"
#include "helpers/vector.h"
#include "helpers/buffer.h"
#include "string.h"
#include "assert.h"
#include <stdlib.h>
#include <ctype.h>

/*
* @fn int lex(LexProcess* lex_process)
* @brief Lexes a file
* @details Performs lexical analysis by reading tokens from the input file, storing them in the token vector, and printing the final token list.
* @param lex_process The lex process
* @return The result of the lex process
*/
int lex(LexProcess* lex_process);
/*
* @var static LexProcess* ptr_to_lex_process
*/
static LexProcess* ptr_to_lex_process;
/*
* @fn Token* read_next_token()
* @brief Reads the next token
* @details Reads the next token from the input stream by checking the current character and applying the appropriate tokenization function. If an unknown character is encountered, an error is reported.
* @return The next token
*/
Token* read_next_token();
/*
* @fn static char peek_char()
* @brief Peeks at the next character
* @details Retrieves the next character from the input stream without advancing the read position by calling the function pointer stored in ptr_to_lex_process->functions->peek_char().
* @return The next character
*/
static char peek_char();
/*
* @fn static Token* make_token_given_number()
* @brief Makes a token given a number
* @details Creates a token for any numeric value by first reading the number using read_number(), then converting it into a token representation using make_token_given_number_as_value().
* @return The token
*/
static Token* make_token_given_number();
/*
* @unsigned long long read_number()
* @brief Reads a number
* @details Reads a numeric string representation using read_number_string() and converts it to an unsigned long long using atoll(), which may cause undefined behavior for negative numbers or values exceeding unsigned long long limits.
* @return The number
*/
unsigned long long read_number();
/*
* @fn static char next_char()
* @brief Gets the next character
* @details Retrieves the next character from the input stream using the function pointer, updates the lexical position (line/column tracking), and appends the character to the expression buffer if currently inside an expression.
* @return The next character
*/
static char next_char();
/*
* @typedef LEX_GETCHAR_IF
* @brief Gets the next character if a condition is met
* @details This function gets the next character if a condition is met
*/
// #define LEX_GETCHAR_IF(buffer, character, expression) \
//     for(character = peek_char(); expression; character = peek_char()){ \
//         append_character_to_buffer(buffer, character); \
//         next_char(); \
//     }
/*
* @fn const char* read_number_string()
* @brief Reads a number string
* @details Reads a sequence of numeric characters from the input stream into a dynamically allocated buffer, ensuring null termination, and returns a pointer to the stored string representation of the number.
* @return The number string
*/
const char* read_number_string();
/*
* @fn Token* make_token_given_number_as_value(unsigned long number)
* @brief Makes a token given a number as a value
* @details Creates a numeric token with the given number value, determining its type based on the next character in the input stream, and advances the lexer if the number has a special type.
* @param number The number
* @return The token
*/
Token* make_token_given_number_as_value(unsigned long number);
/*
* @var static Token temporary_token
* @brief The temporary token
*/
static Token temporary_token;
/*
* @fn Token* create_token(Token* token)
* @brief Creates a token
* @details Copies the given token into a temporary storage, assigns its position from the current lexer state, and attaches the expression buffer content if the token is inside an expression, then returns a pointer to the temporary token.
* @param token The token
* @return The token
*/
Token* create_token(Token* token);
/*
* @fn static PositionInFile lex_file_position()
* @brief Gets the file position
* @details Returns the current position in the file (line and column) from the active lexical analysis process.
* @return The file position
*/
static PositionInFile lex_file_position();
/*
* @fn static Token* lexer_last_token()
* @brief Gets the last token
* @details Retrieves the last token from the token vector, or returns NULL if no tokens have been recorded yet.
* @return The last token
*/
static Token* lexer_last_token();
/*
* @fn static Token* handle_whitespace()
* @brief Handles whitespace
* @details Handles whitespace by marking the last token as having trailing whitespace (if applicable), consuming the whitespace character, and recursively fetching the next meaningful token.
* @return The token
*/
static Token* handle_whitespace();
/*
* @fn static Token* handle_newline()
* @brief Handles a newline
* @details Advances to the next character, then creates and returns a token representing a newline.
* @return The token
*/
static Token* handle_newline();
/*
* @fn void print_token(Token* token)
* @brief Prints a token
* @details Prints detailed information about a given token, including its type, position in the source file, and value (if applicable).
* @param token The token
* @return void
*/
void print_token(Token* token);
/*
* @fn static Token* make_token_given_string(char start_delimiter, char end_delimiter)
* @brief Makes a token given a string
* @details Reads a quoted string from the input, handling escape sequences, and returns a STRING token.
* @param start_delimiter The start delimiter
* @return The token
*/
static Token* make_token_given_string(char start_delimiter, char end_delimiter);
/*
* @fn static Token* make_token_given_operator_or_string()
* @brief Makes a token given an operator or string
* @details Handles token creation for operators or strings, treating <...> as a string if following include, otherwise parsing as an operator.
* @return The token
*/
static Token* make_token_given_operator_or_string();
/*
* @fn const char* read_operator()
* @brief Reads an operator
* @details Reads and constructs an operator token, handling multi-character operators and validating correctness while preserving the first character if invalid.
* @return The operator
*/
const char* read_operator();
/*
* @fn static bool is_operator_treated_as_one_character(char operator)
* @brief Checks if an operator is treated as one character
* @details Check if a single character is an operator (all symbols)
* @param operator The operator
* @return The result
*/
static bool is_operator_treated_as_one_character(char operator);
/*
* @fn static bool is_single_operator(char operator)
* @brief Checks if an operator is single
* @details Check if a single character is one of the basic operators
* @param operator The operator
* @return The result
*/
static bool is_single_operator(char operator);
/*
* @fn static bool is_operator_valid(const char* operator)
* @brief Checks if an operator is valid
* @details This function checks if an operator is valid
* @param operator The operator
* @return The result
*/
static bool is_operator_valid(const char* operator);
/*
* @fn void read_operator_flush_back_but_keep_first_character(BufferType* buffer)
* @brief Reads an operator and flushes back but keeps the first character
* @details Flushes back all but the first character of the operator, preserving the initial character while restoring the rest to the input stream.
* @return void
*/
void read_operator_flush_back_but_keep_first_character(BufferType* buffer);
/*
* @fn static void push_char(char character)
* @brief Pushes a character
* @details Pushes a character back into the lexer’s input stream using the defined lexing process functions.
* @param character The character
* @return void
*/
static void push_char(char character);
/*
* @fn static void lex_new_expression()
* @brief Lexes a new expression
* @details Initializes a new expression by incrementing the expression counter and ensuring the parenthesis buffer exists.
* @return void
*/
static void lex_new_expression();
/*
* @fn bool lex_is_in_expression()
* @brief Checks if the lexer is in an expression
* @details Checks if the lexer is currently inside an expression by verifying the expression count.
* @return The result
*/
bool lex_is_in_expression();
/*
* @fn static void lex_end_expression()
* @brief Ends an expression
* @details Ends the current expression, ensuring the count doesn't go negative, and raises an error for unmatched closing parentheses.
* @return void
*/
static void lex_end_expression();
/*
* @fn static Token* make_token_given_symbol()
* @brief Makes a token given a symbol
* @details Creates a symbol token while handling closing parentheses by ending the current expression.
* @return The token
*/
static Token* make_token_given_symbol();
/*
* @fn Token* make_token_given_identifier_or_keyword()
* @brief Makes a token given an identifier or keyword
* @details Reads an identifier or keyword, storing it in a buffer, and creates a token classified as either a keyword or an identifier.
* @return The token
*/
Token* make_token_given_identifier_or_keyword();
/*
* @fn Token* read_special_token()
* @brief Reads a special token
* @details Attempts to read a special token, returning an identifier or keyword token if the first character is alphabetic or an underscore; otherwise, returns NULL.
* @return The token
*/
Token* read_special_token();
/*
* @fn bool is_keyword(const char* keyword)
* @brief Checks if a string is a keyword
* @details Checks if the given string matches a predefined list of C language keywords.
* @param keyword The keyword
* @return The result
*/
bool is_keyword(const char* keyword);
/*
* @fn Token* make_token_given_one_line_comment()
* @brief Makes a token given a one line comment
* @details Reads and creates a token for a single-line comment by capturing all characters until a newline or EOF.
* @return The token
*/
Token* make_token_given_one_line_comment();
/*
* @fn Token* make_token_given_multi_line_comment()
* @brief Makes a token given a multi line comment
* @details Reads and creates a token for a multi-line comment, handling EOF errors and ensuring proper termination with 
* @return The token
*/
Token* make_token_given_multi_line_comment();
/*
* @fn Token* handle_comment()
* @brief Handles a comment
* @details Handles comment tokens by distinguishing between single-line (//), multi-line, or treating / as an operator if neither applies.
* @return The token
*/
Token* handle_comment();
/*
* @fn Token* make_token_given_quote()
* @brief Makes a token given a quote
* @details Processes a character literal by handling escape sequences and ensuring proper quote closure, then creates a number token.
* @return The token
*/
Token* make_token_given_quote();
/*
* @fn const char lex_get_escape_character(char character)
* @brief Gets an escape character
* @details Converts an escape sequence character into its corresponding actual character representation.
* @param character The character
* @return The escape character
*/
const char lex_get_escape_character(char character);
/*
* @fn static char assert_next_char(char expected)
* @brief Asserts the next character
* @details Ensures the next character matches the expected value, asserting if it doesn't, and returns the character.
* @param expected The expected character
* @return The character
*/
static char assert_next_char(char expected);
/*
* @fn Token* make_token_given_special_number()
* @brief Makes a token given a special number
* @details Handles special number literals (hexadecimal or binary) by checking prefixes (0x or 0b), otherwise defaults to an identifier or keyword.
* @return The token
*/
Token* make_token_given_special_number();
/*
* @fn void lexer_pop_last_token()
* @brief Pops the last token
* @details Removes the last token from the lexer’s token vector.
* @return void
*/
void lexer_pop_last_token();
/*
* @fn Token* make_token_given_special_number_hexadecimal()
* @brief Makes a token given a special number in hexadecimal
* @details Parses a hexadecimal number prefixed with 0x, converts it to an integer, and creates a number token.
* @return The token
*/
Token* make_token_given_special_number_hexadecimal();
/*
* @fn const char* read_hex_number_string()
* @brief Reads a hexadecimal number string
* @details Reads a sequence of hexadecimal characters (0-9, a-f, A-F), stores them in a buffer, null-terminates the string, and returns a pointer to the buffer's memory.
* @return The hexadecimal number string
*/
const char* read_hex_number_string();
/*
* @fn bool is_hex_character(char character)
* @brief Checks if a character is hexadecimal
* @details Checks whether the given character is a valid hexadecimal digit (0-9, a-f, A-F) and returns true if it is, otherwise false.
* @param character The character
* @return The result
*/
bool is_hex_character(char character);
/*
* @fn Token* make_token_given_special_number_binary()
* @brief Makes a token given a special number in binary
* @details Parses a binary number token (e.g., 0b1010) and returns it as a numeric token.
* @return The token
*/
Token* make_token_given_special_number_binary();
/*
* @fn void validate_binary_number(const char* number_string)
* @brief Validates a binary number
* @details Validates that the given string contains only '0' and '1' for a binary number.
* @param number_string The number string
* @return void
*/
void validate_binary_number(const char* number_string);
/*
* @var LexProcessFunctions lexer_string_buffer_functions
*/
LexProcessFunctions lexer_string_buffer_functions;
/*
* @fn char lexer_string_buffer_next_char(LexProcess* lex_process)
* @brief Gets the next character from a string buffer
* @details Reads the next character from the string buffer, returning EOF if the end is reached.
* @param lex_process The lex process
* @return The next character
*/
char lexer_string_buffer_next_char(LexProcess* lex_process);
/*
* @fn lexer_string_buffer_peek_char(LexProcess* lex_process)
* @brief Peeks at the next character from a string buffer
* @details Peeks at the next character in the string buffer without advancing, returning EOF if at the end.
* @param lex_process The lex process
* @return The next character
*/
char lexer_string_buffer_peek_char(LexProcess* lex_process);
/*
* @fn void lexer_string_buffer_push_char(LexProcess* lex_process, char character)
* @brief Pushes a character to a string buffer
* @details Pushes a character back into the string buffer for reprocessing.
* @param lex_process The lex process
* @param character The character
* @return void
*/
void lexer_string_buffer_push_char(LexProcess* lex_process, char character);
/*
* @fn int lexer_number_type(char character)
* @brief Gets the number type
* @details // Determines the type of a numeric literal based on its suffix character.
* @param character The character
* @return The number type
*/
int lexer_number_type(char character);

int lex(LexProcess* lex_process){
    lex_process->current_expression_count = 0;
    lex_process->parenthesis_buffer = NULL;
    ptr_to_lex_process = lex_process;
    lex_process->position.file_name = lex_process->compiler->input_file.absolute_path;

    Token* token = read_next_token();
    while(token){
        push_element(lex_process->token_vector, token);
        token = read_next_token();
    }
    // print_token_vector(lex_process->token_vector);
    return LEXICAL_ANALYSIS_SUCCESS;
}


Token* read_next_token(){
    Token* token = NULL;
    char character = peek_char();
    token = handle_comment();
    if(token){
        return token;
    }
    switch(character){
        NUMERIC_CASES:
            token = make_token_given_number();
            break;
        OPERATOR_CASE_EXCLUDING_DIVISION:
            token = make_token_given_operator_or_string();
            break;
        SYMBOL_CASE:
            token = make_token_given_symbol();
            break;
        case 'x':
            token = make_token_given_special_number();
            break;
        case 'b':
            token = make_token_given_special_number();
            break;
        case '"':
            token = make_token_given_string('"', '"');
            break;
        case ' ':
        case '\t':
            token = handle_whitespace();
            break;
        case EOF: //end of file, finished lexing
            break;
        case '\n':
            token = handle_newline();
            break;
        case '\'':
            token = make_token_given_quote();
            break;
        default:
            token = read_special_token();
            if(token == NULL){
                compiler_error(ptr_to_lex_process->compiler, "Unknown character");
            }
    }
    return token;
}

static char peek_char(){
    return ptr_to_lex_process->functions->peek_char(ptr_to_lex_process);
}

static Token* make_token_given_number(){
    //any type of number!
    return make_token_given_number_as_value(read_number());
}

unsigned long long read_number(){
    const char* number_string = read_number_string();
    return atoll(number_string);
}

static char next_char(){
    char character = ptr_to_lex_process->functions->next_char(ptr_to_lex_process);
    if(lex_is_in_expression()){
        append_character_to_buffer(ptr_to_lex_process->parenthesis_buffer, character);
    }
    ptr_to_lex_process->position.column++;
    if(character == '\n'){
        ptr_to_lex_process->position.line++;
        ptr_to_lex_process->position.column = 1;
    }
    return character;
}

#define LEX_GETCHAR_IF(buffer, character, expression) \
    for(character = peek_char(); expression; character = peek_char()){ \
        append_character_to_buffer(buffer, character); \
        next_char(); \
    }

const char* read_number_string(){
    const char* number = NULL;
    BufferType* buffer = create_buffer();
    char character = peek_char();
    LEX_GETCHAR_IF(buffer, character, (character >= '0' && character <= '9'));
    append_character_to_buffer(buffer, 0x00); //null terminate the string
    return get_buffer_memory_pointer(buffer);    
}


Token* make_token_given_number_as_value(unsigned long number){
    int number_type = lexer_number_type(peek_char());
    if(number_type!=NUMBER_TYPE_NORMAL_INT){
        next_char();
    }
    return create_token(&(Token){
        .type = TOKEN_TYPE_NUMBER,
        .value.long_long_num = number,
        .Number.type = number_type,
    });
}

static Token temporary_token;

Token* create_token(Token* token){
    memcpy(&temporary_token, token, sizeof(Token));
    temporary_token.position = lex_file_position();
    if(lex_is_in_expression()){
        temporary_token.whats_between_brackets = get_buffer_memory_pointer(ptr_to_lex_process->parenthesis_buffer);
    }
    return &temporary_token;
}

static PositionInFile lex_file_position(){
    return ptr_to_lex_process->position;
}

static Token* handle_whitespace() {
    Token* last_token = lexer_last_token();
    if(last_token){
        last_token->is_whitespace = true;
    }
    next_char();
    return read_next_token();
}

static Token* lexer_last_token(){
    return get_last_element_or_null(ptr_to_lex_process->token_vector);
}

static Token* handle_newline(){
    next_char();
    return create_token(&(Token){
        .type = TOKEN_TYPE_NEWLINE,
    });
}

void print_token(Token* token){ 
    printf("Token position: %s:%i:%i\n", token->position.file_name, token->position.line, token->position.column);
    switch(token->type){
        case TOKEN_TYPE_NUMBER:
            printf("Token number: %llu\n", token->value.long_long_num);
            break;
        case TOKEN_TYPE_STRING:
            printf("Token string: %s\n", token->value.string_val);
            break;
        //keep adding cases for other token types
        case TOKEN_TYPE_NEWLINE:
            printf("Token newline\n");
            break;
        case TOKEN_TYPE_OPERATOR:
            printf("Token operator: %s\n", token->value.string_val);
            break;
        case TOKEN_TYPE_SYMBOL:
            printf("Token symbol: %c\n", token->value.char_val);
            break;
        case TOKEN_TYPE_IDENTIFIER:
            printf("Token identifier: %s\n", token->value.string_val);
            break;
        case TOKEN_TYPE_KEYWORD:
            printf("Token keyword: %s\n", token->value.string_val);
            break;
        case TOKEN_TYPE_COMMENT:
            printf("Token comment: %s\n", token->value.string_val);
            break;
        case EOF:
            //printf("End of file\n");
            break;
        default:
            printf("Unknown token type\n");
    }
}

static Token* make_token_given_string(char start_delimiter, char end_delimiter){
    BufferType* buffer = create_buffer();
    assert(next_char() == start_delimiter);
    char character = next_char();
    while(character != end_delimiter && character != EOF){
        if(character == '\\'){
            //handle escape characters
            continue;
        }
        append_character_to_buffer(buffer, character);
        character = next_char();
    }
    append_character_to_buffer(buffer, 0x00); //null terminate the string
    return create_token(&(Token){
        .type = TOKEN_TYPE_STRING,
        .value.string_val = get_buffer_memory_pointer(buffer),
    });
}

static Token* make_token_given_operator_or_string(){
    char character = peek_char();
    if(character == '<'){
        Token* last_token = lexer_last_token();
        if(is_token_keyword(last_token, "include")){
            return make_token_given_string('<', '>');
        }

    }
    Token* token = create_token(&(Token){
        .type = TOKEN_TYPE_OPERATOR,
        .value.string_val = read_operator(),
    });
    if(character == '('){
        lex_new_expression();
    }
    return token;
}

const char* read_operator() {
    char character = next_char();
    BufferType* buffer = create_buffer();
    append_character_to_buffer(buffer, character);

    // Peek at the next character to check for multi-character operators
    char next_character = peek_char();

    if (is_single_operator(next_character)) {  // Check if the next char is also an operator
        append_character_to_buffer(buffer, next_character);
        next_char();  // Consume it
    }

    append_character_to_buffer(buffer, 0x00);  // Null-terminate

    char* operator = get_buffer_memory_pointer(buffer);

    if (!is_operator_valid(operator)) {
        read_operator_flush_back_but_keep_first_character(buffer);
        operator[1] = 0x00;
    }

    return operator;
}


// Check if a single character is an operator (all symbols)
static bool is_operator_treated_as_one_character(char op) {
    return op == '+'  || op == '-'  || op == '*'  || op == '/'  || op == '%'  ||
           op == '='  || op == '!'  || op == '&'  || op == '|'  || op == '^'  ||
           op == '<'  || op == '>'  || op == '?'  || op == '~'  ||
           op == '['  || op == '('  ||
           op == ','  || op == '.'  ;
}

// Check if a single character is one of the basic operators
static bool is_single_operator(char op) {
    return op == '+'  || op == '-'  || op == '*'  || op == '/'  || op == '%'  ||
           op == '='  || op == '!'  || op == '&'  || op == '|'  || op == '^'  ||
           op == '<'  || op == '>'  || op == '?'  || op == '~'  ||
           op == '['  || op == '('  ||
           op == ','  || op == '.'  ;
}

// Check if a string (operator token) is valid
static bool is_operator_valid(const char* op) {
    return ARE_STRINGS_EQUAL(op, "+")   || ARE_STRINGS_EQUAL(op, "-")   ||
           ARE_STRINGS_EQUAL(op, "*")   || ARE_STRINGS_EQUAL(op, "/")   ||
           ARE_STRINGS_EQUAL(op, "%")   || ARE_STRINGS_EQUAL(op, "=")   ||
           ARE_STRINGS_EQUAL(op, "!")   || ARE_STRINGS_EQUAL(op, "&")   ||
           ARE_STRINGS_EQUAL(op, "|")   || ARE_STRINGS_EQUAL(op, "^")   ||
           ARE_STRINGS_EQUAL(op, "<")   || ARE_STRINGS_EQUAL(op, ">")   ||
           ARE_STRINGS_EQUAL(op, "?")   || ARE_STRINGS_EQUAL(op, "~")   ||
           ARE_STRINGS_EQUAL(op, "[")   ||
           ARE_STRINGS_EQUAL(op, "(")   ||
           ARE_STRINGS_EQUAL(op, "{")   ||
           ARE_STRINGS_EQUAL(op, ",")   || ARE_STRINGS_EQUAL(op, ".")   ||
           ARE_STRINGS_EQUAL(op, ":")   || ARE_STRINGS_EQUAL(op, ";")   ||

           // Multi-character operators
           ARE_STRINGS_EQUAL(op, "+=")  || ARE_STRINGS_EQUAL(op, "-=")  ||
           ARE_STRINGS_EQUAL(op, "*=")  || ARE_STRINGS_EQUAL(op, "/=")  ||
           ARE_STRINGS_EQUAL(op, "%=")  || ARE_STRINGS_EQUAL(op, "==")  ||
           ARE_STRINGS_EQUAL(op, "!=")  || ARE_STRINGS_EQUAL(op, "&&")  ||
           ARE_STRINGS_EQUAL(op, "||")  || ARE_STRINGS_EQUAL(op, "++")  ||
           ARE_STRINGS_EQUAL(op, "--")  || ARE_STRINGS_EQUAL(op, "<<")  ||
           ARE_STRINGS_EQUAL(op, ">>")  || ARE_STRINGS_EQUAL(op, "<=")  ||
           ARE_STRINGS_EQUAL(op, ">=")  ||

           ARE_STRINGS_EQUAL(op, "<<=") || ARE_STRINGS_EQUAL(op, ">>=") ||
           ARE_STRINGS_EQUAL(op, "->")  || ARE_STRINGS_EQUAL(op, "->*") ||
           ARE_STRINGS_EQUAL(op, "::")  || ARE_STRINGS_EQUAL(op, ".*")  ||
           ARE_STRINGS_EQUAL(op, "...") || ARE_STRINGS_EQUAL(op, "<=>") ||
           ARE_STRINGS_EQUAL(op, "?:");
}

void read_operator_flush_back_but_keep_first_character(BufferType* buffer){
    const char* data = get_buffer_memory_pointer(buffer);
    int length = buffer->current_length;
    for(int i = length - 1; i>=1; i--){
        if(data[i] == 0x00){
            continue;
        }
        push_char(data[i]);
    }
}

static void push_char(char character){
    ptr_to_lex_process->functions->push_char(ptr_to_lex_process, character);
}

static void lex_new_expression(){
    ptr_to_lex_process->current_expression_count++;
    if(ptr_to_lex_process->parenthesis_buffer == NULL){
        ptr_to_lex_process->parenthesis_buffer = create_buffer();
    }
}

bool lex_is_in_expression(){
    return ptr_to_lex_process->current_expression_count > 0;
}

static void lex_end_expression(){
    ptr_to_lex_process->current_expression_count--;
    if(ptr_to_lex_process->current_expression_count < 0){
        compiler_error(ptr_to_lex_process->compiler, "Unmatched parenthesis / you closed the expression you never opened");
    }
}

static Token* make_token_given_symbol(){
    char character = next_char();
    if(character == ')'){
        lex_end_expression();
    }
    return create_token(&(Token){
        .type = TOKEN_TYPE_SYMBOL,
        .value.char_val = character,
    });
}

Token* make_token_given_identifier_or_keyword(){
    BufferType* buffer = create_buffer();
    char character = 0;
    LEX_GETCHAR_IF(buffer, character, (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == '_');
    append_character_to_buffer(buffer, 0x00); //null terminate the string
    if(is_keyword(get_buffer_memory_pointer(buffer))){
        return create_token(&(Token){
            .type = TOKEN_TYPE_KEYWORD,
            .value.string_val = get_buffer_memory_pointer(buffer),
        });
    }
    return create_token(&(Token){
        .type = TOKEN_TYPE_IDENTIFIER,
        .value.string_val = get_buffer_memory_pointer(buffer),
    });
}

Token* read_special_token(){ //when unsure if the token is identifier or a keyword
    char character = peek_char();
    if(isalpha(character) || character == '_'){
        return make_token_given_identifier_or_keyword();
    }
    return NULL;
}

bool is_keyword(const char* keyword){
    //unsigned, signed, char, short, int, float, double, long, void, struct, union, static, _ignore_typecheck, return, include, sizeof, if, else, while, for, do, break, continue, switch, case, default, goto, typedef, const, extern, restrict
    return ARE_STRINGS_EQUAL(keyword, "unsigned") || ARE_STRINGS_EQUAL(keyword, "signed") || ARE_STRINGS_EQUAL(keyword, "char") || ARE_STRINGS_EQUAL(keyword, "short") || ARE_STRINGS_EQUAL(keyword, "int") || ARE_STRINGS_EQUAL(keyword, "float") || ARE_STRINGS_EQUAL(keyword, "double") || ARE_STRINGS_EQUAL(keyword, "long") || ARE_STRINGS_EQUAL(keyword, "void") || ARE_STRINGS_EQUAL(keyword, "struct") || ARE_STRINGS_EQUAL(keyword, "union") || ARE_STRINGS_EQUAL(keyword, "static") || ARE_STRINGS_EQUAL(keyword, "_ignore_typecheck") || ARE_STRINGS_EQUAL(keyword, "return") || ARE_STRINGS_EQUAL(keyword, "include") || ARE_STRINGS_EQUAL(keyword, "sizeof") || ARE_STRINGS_EQUAL(keyword, "if") || ARE_STRINGS_EQUAL(keyword, "else") || ARE_STRINGS_EQUAL(keyword, "while") || ARE_STRINGS_EQUAL(keyword, "for") || ARE_STRINGS_EQUAL(keyword, "do") || ARE_STRINGS_EQUAL(keyword, "break") || ARE_STRINGS_EQUAL(keyword, "continue") || ARE_STRINGS_EQUAL(keyword, "switch") || ARE_STRINGS_EQUAL(keyword, "case") || ARE_STRINGS_EQUAL(keyword, "default") || ARE_STRINGS_EQUAL(keyword, "goto") || ARE_STRINGS_EQUAL(keyword, "typedef") || ARE_STRINGS_EQUAL(keyword, "const") || ARE_STRINGS_EQUAL(keyword, "extern") || ARE_STRINGS_EQUAL(keyword, "restrict");
}

Token* make_token_given_one_line_comment(){
    BufferType* buffer = create_buffer();
    char character = 0;
    LEX_GETCHAR_IF(buffer, character, character != '\n' && character != EOF);
    return create_token(&(Token){
        .type = TOKEN_TYPE_COMMENT,
        .value.string_val = get_buffer_memory_pointer(buffer),
    });
}

Token* make_token_given_multi_line_comment(){
    BufferType* buffer = create_buffer();
    char character = 0;
    while(true){
        LEX_GETCHAR_IF(buffer, character, character != '*' && character != EOF);
        if(character == EOF){
            compiler_error(ptr_to_lex_process->compiler, "Unexpected end of file in multi-line comment");
        }
        else if(character == '*'){
            next_char();
            if(peek_char() == '/'){
                next_char();
                break;
            }
        }
    }
    return create_token(&(Token){
        .type = TOKEN_TYPE_COMMENT,
        .value.string_val = get_buffer_memory_pointer(buffer),
    });
}

Token* handle_comment(){
    char character = peek_char();
    if(character == '/'){
        next_char();
        if(peek_char() == '/'){
            next_char();
            return make_token_given_one_line_comment();
        }
        else if(peek_char() == '*'){
            next_char();
            return make_token_given_multi_line_comment();
        }
        else{
            push_char('/');
            return make_token_given_operator_or_string();
        }
    }
    return NULL;
}

Token* make_token_given_quote(){
    assert_next_char('\'');
    char character = next_char();
    if(character == '\\'){
        //handle escape characters
        character = next_char();
        character = lex_get_escape_character(character);

    }
    if(next_char() != '\''){
        compiler_error(ptr_to_lex_process->compiler, "Did not closed the opened quote");
    }
    return create_token(&(Token){
        .type = TOKEN_TYPE_NUMBER,
        .value.char_val = character,
    });
}

const char lex_get_escape_character(char character){
    char co = 0;
    switch(character){
        case 'n':
            co = '\n';
            break;
        case '\\':
            co = '\\';
            break;
        case 't':
            co = '\t';
            break;
        case '\'':
            co = '\'';
            break;
    }
    return co; 
}

static char assert_next_char(char expected){
    char character = next_char();
    assert(character == expected);
    return character;
}

Token* make_token_given_special_number(){
    Token* token = NULL;
    Token* last_token = lexer_last_token();
    if(!last_token || !(last_token->type == TOKEN_TYPE_OPERATOR && last_token->value.long_long_num == 0)){
        return make_token_given_identifier_or_keyword();
    }
    lexer_pop_last_token();
    char character = peek_char();
    if(character == 'x'){
        token = make_token_given_special_number_hexadecimal();
    }
    else if(character == 'b'){
        token = make_token_given_special_number_binary();
    }
}

void lexer_pop_last_token(){
    remove_last_element(ptr_to_lex_process->token_vector);
}

Token* make_token_given_special_number_hexadecimal(){
    next_char();
    unsigned long number = 0;
    const char* number_string = read_hex_number_string();
    number = strtol(number_string, NULL, 16);
    return create_token(&(Token){
        .type = TOKEN_TYPE_NUMBER,
        .value.long_long_num = number,
    });
}

const char* read_hex_number_string(){
    BufferType* buffer = create_buffer();
    char character = peek_char();
    LEX_GETCHAR_IF(buffer, character, is_hex_character(character));
    append_character_to_buffer(buffer, 0x00); //null terminate the string
    return get_buffer_memory_pointer(buffer);
}

bool is_hex_character(char character){
    return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F');
}

Token* make_token_given_special_number_binary(){
    next_char();
    unsigned long number = 0;
    const char* number_string = read_number_string();
    validate_binary_number(number_string);
    number = strtol(number_string, NULL, 2);
    return make_token_given_number_as_value(number);
}

void validate_binary_number(const char* number_string){
    for(int i = 0; number_string[i] != 0x00; i++){
        if(number_string[i] != '0' && number_string[i] != '1'){
            compiler_error(ptr_to_lex_process->compiler, "Invalid binary number");
        }
    }
}

// Converts an input string into tokens for lexical analysis, similar to yy_scan_string in Flex.
LexProcess* build_tokens_for_string(CompileProcess* compiler, const char* string){ //equivalent to yy_scan_string in flex
    BufferType* buffer = create_buffer();
    append_formatted_text(buffer, string);
    LexProcess* lex_process = create_lex_process(compiler, &lexer_string_buffer_functions, buffer);
    if(!lex_process){
        return NULL;
    }
    if(lex(lex_process) != LEXICAL_ANALYSIS_SUCCESS){
        free_lex_process(lex_process);
        return NULL;
    }
    return lex_process;
}

LexProcessFunctions lexer_string_buffer_functions = {
    .next_char = lexer_string_buffer_next_char,
    .peek_char = lexer_string_buffer_peek_char,
    .push_char = lexer_string_buffer_push_char,
};

char lexer_string_buffer_next_char(LexProcess* lex_process){
    BufferType* buffer = get_private_data_of_lex_process(lex_process);
    char character = read_character_from_buffer(buffer);
    if(character == 0x00){
        return EOF;
    }
    return character;
}

char lexer_string_buffer_peek_char(LexProcess* lex_process){
    BufferType* buffer = get_private_data_of_lex_process(lex_process);
    char character = peek_character_in_buffer(buffer);
    if(character == 0x00){
        return EOF;
    }
    return character;
}

void lexer_string_buffer_push_char(LexProcess* lex_process, char character){
    BufferType* buffer = get_private_data_of_lex_process(lex_process);
    append_character_to_buffer(buffer, character);
}


int lexer_number_type(char character){
    int result = NUMBER_TYPE_NORMAL_INT;
    if(character == 'L'){
        result = NUMBER_TYPE_LONG;
    }
    else if(character == 'f'){
        result = NUMBER_TYPE_FLOAT;
    }
    else if(character == 'd'){
        result = NUMBER_TYPE_DOUBLE;
    }
    return result;
}

//yylex() Equivalent, it gets the next token
Token* yylex(){
    return read_next_token();
}

//yytext equivalent, it gets the string value of the token
const char* yytext(){
    return temporary_token.value.string_val;
}

//yyleng equivalent, it gets the length of the string value of the token
int yyleng(){
    return strlen(temporary_token.value.string_val);
}