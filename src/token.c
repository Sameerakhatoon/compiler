/*
* @file token.c
* @brief The token
* @details This file contains the token
*/

#include "compiler.h"

bool is_token_keyword(Token* token, const char* value);
bool parser_ignore_nl_or_comment_or_nl_seperator_tokens(Token* token);
bool is_token_symbol(Token* token, char symbol);


/*
* @fn bool is_token_keyword(Token* token, const char* value)
* @brief Checks if a token is a keyword
* @details Checks if a token is a keyword and matches the given value.
* @param token The token
* @param value The value
* @return The result
*/
bool is_token_keyword(Token* token, const char* value){
    return token && token->type == TOKEN_TYPE_KEYWORD && ARE_STRINGS_EQUAL(token->value.string_val, value);
}

bool parser_ignore_nl_or_comment_or_nl_seperator_tokens(Token* token){
    if(!token){
        return false;
    }
    return token->type == TOKEN_TYPE_NEWLINE || token->type == TOKEN_TYPE_COMMENT || is_token_symbol(token, '\\');
}
/*
* @fn bool is_token_symbol(Token* token, char symbol)
* @brief Checks if a token is a symbol
* @details Checks if a token is a symbol and matches the given character.
* @param token The token
* @param symbol The symbol
* @return The result
*/
bool is_token_symbol(Token* token, char symbol){
    return token && token->type == TOKEN_TYPE_SYMBOL && token->value.char_val == symbol;
}

bool is_token_primitive_keyword(Token* token){
    return token && token->type == TOKEN_TYPE_KEYWORD && (ARE_STRINGS_EQUAL(token->value.string_val, "int") || ARE_STRINGS_EQUAL(token->value.string_val, "char") || ARE_STRINGS_EQUAL(token->value.string_val, "float") || ARE_STRINGS_EQUAL(token->value.string_val, "double") || ARE_STRINGS_EQUAL(token->value.string_val, "void") || ARE_STRINGS_EQUAL(token->value.string_val, "long") || ARE_STRINGS_EQUAL(token->value.string_val, "short"));
}