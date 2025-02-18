#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>

static CompileProcess* current_process;

int parse(CompileProcess* compiler);
int parse_next_token();
static Token* parser_last_token;

Token* get_next_token();
static void parser_ignore_nl_or_comment_tokens(Token* token);
Token* peek_next_token();
void parse_single_token_to_node();
Node* create_node(Node* node);

void print_node_vector(DynamicVector* node_vector);

typedef struct History{ //so we'd be able to passs down commands to recursive functions
    int flags;
} History;

History* begin_history(int flags);

History* clone_history(History* history, int flags);

void parse_expressionable(History* history);

int parse_expressionable_single(History* history);

int parse_expression(History* history);

void parse_normal_expression(History* history);

void parse_operator_expression(History* history, const char* operator);


// extern ExpressionableOperatorPrecedanceGroup operator_precedence[TOTAL_OPERATOR_GROUPS];

void parser_reorder_expression(Node** node_out);

static bool does_left_operator_have_higher_precedence(char* left_operator, char* right_operator);

static int parser_get_precedence_for_operator(const char* operator, ExpressionableOperatorPrecedanceGroup** group_out);

void parser_node_shift_right_to_left(Node* node);

void parse_identifier(History* history);

void parse_keyword(History* history);

static bool is_keyword_variable_modifier(const char* value);

bool keyword_is_datatype(const char* value);

void parse_variable_or_function_or_struct_or_union(History* history);

void parse_datatype(DataType* datatype);

void parse_datatype_modifier(DataType* data_type);

void parse_datatype_type(DataType* data_type);

void parser_get_datatype_tokens(Token** datatype_token_out, Token** datatype_token_secondary_out);

int parser_datatype_expected_for_type_string(const char* value);

Token* parser_assign_random_name_to_struct_or_union();

int parser_get_random_type_index();

int parser_get_pointer_level();

bool is_next_token_operator(const char* operator);

void parser_datatype_init(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out, int pointer_depth, int expected_type);

void parser_datatype_init_type_and_size(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out, int pointer_depth, int expected_type);

bool is_secondary_datatype_allowed(int expected_type);

bool is_secondary_datatype_allowed_for_type(const char* expected_type);

void parser_datatype_init_type_and_size_for_primitive(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out);

void parser_adjust_size_for_secondary_datatype(DataType* datatype, Token* datatype_secondary_token);

void parse_keyword_for_global();

void parser_ignore_int(DataType* datatype);

bool parser_is_init_valid_after_datatype(DataType* datatype);

void parse_variable(DataType* datatype, Token* name_token, History* history);

void parse_expressionable_root(History* history);

void make_variable_node_and_register(History* history, DataType* datatype, Token* name_token, Node* value_node);

void make_variable_node(DataType* datatype, Token* name_token, Node* value_node);


void make_variable_list_node(DynamicVector* variable_list);

static void expect_symbol(const char symbol);


ArrayBrackets* parse_array_brackets(History* history);

static void expect_operator(const char* operator);

void make_bracket_node(Node* expression_node);


static bool is_next_token_symbol(const char symbol);
void parse_struct_or_union(DataType* datatype);

void parse_struct(DataType* datatype);

void parser_new_scope();
void parse_struct_no_new_scope(DataType* datatype, bool is_foward_declaration);

void parser_finish_scope();

void parse_body(size_t* sum_of_var_size, History* history);

extern Node* parser_current_body_node;

void parse_body_single_statement(size_t* sum_of_var_size, DynamicVector* body_vector, History* history);

void make_body_node(DynamicVector* body_vector, size_t sum_of_var_size, bool padded, Node* largest_var_node);

void parse_statement(History* history);

void parse_symbol();

void parser_apppend_size_for_node(History* history, size_t* variable_size, Node* node);

void parser_finalize_body(History* history, Node* body_node, DynamicVector* body_vector, size_t* sum_of_var_size, Node* largest_align_eligible_var_node, Node* largest_possible_var_node);

void parse_union(DataType* datatype);


void parser_apppend_size_for_node_struct_or_union(History* history, size_t* variable_size, Node* node);

void parser_apppend_size_for_variable_list(History* history, size_t* variable_size, DynamicVector* variable_list);


// enum{
//     PARSER_SCOPE_ENTITY_ON_STACK = 0b00000001,
//     PARSER_SCOPE_ENTITY_STRUCTURE_SCOPE = 0b00000010,

// };

typedef struct ParserScopeEntity ParserScopeEntity;

ParserScopeEntity* create_new_parser_scope_entity(Node* variable_node, int stack_offset, int flags);

ParserScopeEntity* get_parser_scope_last_entity_stop_global_scope();

void push_parser_scope(ParserScopeEntity* entity, size_t size);

void parser_scope_offset_calculate(History* history, Node* variable_node);

void parser_scope_offset_calculate_for_stack(History* history, Node* variable_node);

void parser_scope_offset_calculate_for_global(History* history, Node* node);

ParserScopeEntity* get_parser_scope_last_entity();

void parser_scope_offset_calculate_for_struct(History* history, Node* node);

void parse_body_multiple_statements(size_t* sum_of_var_size, DynamicVector* body_vector, History* history);

size_t get_size_of_struct(const char* name);










int parse(CompileProcess* compiler){
    create_root_scope(compiler);
    current_process = compiler;
    set_node_vectors(current_process->node_vector, current_process->node_tree_vector);
    parser_last_token = NULL;
    Node* node = NULL;
    set_peek_index(current_process->token_vector, 0);
    while(parse_next_token() == 0){
        //parse the next token
        node = peek_node(); //at this point, the node should be the last node, which is the root of the tree push_element(current_process->node_tree_vector, &node);
    }
    return PARSER_SUCCESS;
}

int parse_next_token(){
    Token* token = peek_next_token();
    if(token == NULL){
        return -1;
    }
    switch(token->type){
        case TOKEN_TYPE_NUMBER:
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_IDENTIFIER:
            parse_expressionable(begin_history(0));
            break;
        case TOKEN_TYPE_KEYWORD:
            parse_keyword_for_global();
            break;
        case TOKEN_TYPE_SYMBOL:
            parse_symbol();
            break;
    }
    return 0;
}

// static Token* parser_last_token;

Token* get_next_token(){
    Token* next_token = peek_element_without_increment(current_process->token_vector);
    if(!next_token){
        return NULL;
    }
    parser_ignore_nl_or_comment_tokens(next_token);
    current_process->position = next_token->position;
    parser_last_token = next_token;
    return peek_element(current_process->token_vector);
}

static void parser_ignore_nl_or_comment_tokens(Token* token){
    while(token && parser_ignore_nl_or_comment_or_nl_seperator_tokens(token)){
        //skip the token
        peek_element(current_process->token_vector);
        token = peek_element_without_increment(current_process->token_vector);
    }
}

Token* peek_next_token(){
    Token* next_token = peek_element_without_increment(current_process->token_vector);
    parser_ignore_nl_or_comment_tokens(next_token);
    return peek_element_without_increment(current_process->token_vector);
}

void parse_single_token_to_node(){
    Token* token = get_next_token();    
    Node* node = NULL;
    switch(token->type){
        case TOKEN_TYPE_NUMBER:
            node = create_node(&((Node){.type = NODE_TYPE_NUMBER, .literal_value.long_long_num = token->value.long_long_num}));
            break;
        case TOKEN_TYPE_STRING:
            node = create_node(&((Node){.type = NODE_TYPE_STRING, .literal_value.string_val = token->value.string_val}));
            break;
        case TOKEN_TYPE_IDENTIFIER:
            node = create_node(&((Node){.type = NODE_TYPE_IDENTIFIER, .literal_value.string_val = token->value.string_val}));
            break;
        default:
            compiler_error(current_process, "this isn't single token, that can be parsed to node");
    }
}

Node* create_node(Node* node){
    Node* node_created = malloc(sizeof(Node));
    memcpy(node_created, node, sizeof(Node));
    #warning "set the binded to"
    push_node(node_created);
    return node_created;
}

void print_node_vector(DynamicVector* node_vector) {
    printf("size of node vector: %i\n", get_element_count(node_vector));
    for (int i = 0; i < get_element_count(node_vector); i++) {
        print_node(*(Node**)get_element_at(node_vector, i), 0);
    }
}

// typedef struct History{
//     int flags;
// } History;

History* begin_history(int flags){
    History* history = calloc(1, sizeof(History));
    history->flags = flags;
    return history;
}

History* clone_history(History* history, int flags){
    History* new_history = calloc(1, sizeof(History));
    memcpy(new_history, history, sizeof(History));
    new_history->flags = flags;
    return new_history;
}

void parse_expressionable(History* history){
    //goes through the tokens and parse them to nodes, form expressions nodes
    while(parse_expressionable_single(history) == 0){
        //parse the next token
    }
}

int parse_expressionable_single(History* history){
    Token* token = peek_next_token();
    if(token == NULL){
        return -1;
    }
    history->flags |= NODE_FLAG_INSIDE_EXPRESSION;
    int result = -1;
    switch(token->type){
        case TOKEN_TYPE_NUMBER:
            parse_single_token_to_node();
            result = 0;
            break;
        case TOKEN_TYPE_IDENTIFIER:
            parse_identifier(history);
            break;
        case TOKEN_TYPE_OPERATOR:
            parse_expression(history);
            result = 0;
            break;
        case TOKEN_TYPE_KEYWORD:
            parse_keyword(history);
            result = 0;
            break;
    }
    return result;
}

int parse_expression(History* history){ //parsing operator & merging w/ correct operands
    parse_normal_expression(history);
    return 0;
}

void parse_normal_expression(History* history){
    Token* token = peek_next_token(); //this is the operator
    char* operator = token->value.string_val;
    Node* left_node = peek_node_expressionable_or_null();
    if(!left_node){
        return;
    }
    get_next_token(); //pop the operator
    pop_node(); //pop the left node
    left_node -> flags |= NODE_FLAG_INSIDE_EXPRESSION;
    //check for expression, paranthesis, nested expression etc
    parse_operator_expression(clone_history(history, history->flags), operator);//parse the right node
    Node* right_node = pop_node();
    right_node -> flags |= NODE_FLAG_INSIDE_EXPRESSION;
    make_expression_node(left_node, right_node, operator);
    Node* expression_node = pop_node();

    //reorder expression according to precedence
    parser_reorder_expression(&expression_node);
    push_node(expression_node);
}

void parse_operator_expression(History* history, const char* operator){ //for operator associativity, precedence
    parse_expressionable(history);
}

extern ExpressionableOperatorPrecedanceGroup operator_precedence[TOTAL_OPERATOR_GROUPS];

void parser_reorder_expression(Node** node_out){ //node_out refers to the expression node
    Node* node = *node_out;
    if(node->type != NODE_TYPE_EXPRESSION){
        return;
    }
    if(node->data.expression.left->type != NODE_TYPE_EXPRESSION && node->data.expression.right && node->data.expression.right->type != NODE_TYPE_EXPRESSION){
        return;
    }
    //50*EXP
    //EXP(50*EXP(10+20))
    //EXP(EXP(50*10)+20)
    //shift child operator(right) to root operator(left)
    if(node->data.expression.left->type != NODE_TYPE_EXPRESSION && node->data.expression.right && node->data.expression.right->type == NODE_TYPE_EXPRESSION){
        char* right_operator = node->data.expression.right->data.expression.operator;
        //functions to determine the priority of the operator, is it the root operator(* in eg) or the child operator(+ in eg)
        if(does_left_operator_have_higher_precedence(node->data.expression.operator, right_operator)){
            parser_node_shift_right_to_left(node);
            parser_reorder_expression(&node->data.expression.left);
            parser_reorder_expression(&node->data.expression.right);
        }
    }

}

static bool does_left_operator_have_higher_precedence(char* left_operator, char* right_operator){
    ExpressionableOperatorPrecedanceGroup* left_group = NULL;
    ExpressionableOperatorPrecedanceGroup* right_group = NULL;
    if(ARE_STRINGS_EQUAL(left_operator, right_operator)){
        return false;
    }
    int left_precedence = parser_get_precedence_for_operator(left_operator, &left_group);
    int right_precedence = parser_get_precedence_for_operator(right_operator, &right_group);
    if(left_group->associativity == ASSOCIATIVITY_RIGHT_TO_LEFT){
        return false;
    }
    return left_precedence <= right_precedence;
}

static int parser_get_precedence_for_operator(const char* operator, ExpressionableOperatorPrecedanceGroup** group_out){
    *group_out = NULL;
    for(int i = 0; i < TOTAL_OPERATOR_GROUPS; i++){
        for(int j = 0; operator_precedence[i].operators[j]; j++){
            const char* operator_in_group = operator_precedence[i].operators[j];
            if(ARE_STRINGS_EQUAL(operator, operator_in_group)){
                *group_out = &operator_precedence[i];
                return i;
            }
        }
    }
    return -1;
}

void parser_node_shift_right_to_left(Node* node){
    assert(node->type == NODE_TYPE_EXPRESSION);
    assert(node->data.expression.right->type == NODE_TYPE_EXPRESSION);

    const char* right_operator = node->data.expression.right->data.expression.operator;
    Node* new_left = node->data.expression.left;
    Node* new_right = node->data.expression.right->data.expression.left;
    make_expression_node(new_left, new_right, node->data.expression.operator);
    //EXP(50*10)
    Node* new_left_operand = pop_node();
    //20
    Node* new_right_operand = node->data.expression.right->data.expression.right;
    node->data.expression.left = new_left_operand;
    node->data.expression.right = new_right_operand;
    node->data.expression.operator = right_operator;
}

void parse_identifier(History* history){
    assert(peek_next_token()->type == TOKEN_TYPE_IDENTIFIER);
    parse_single_token_to_node();
}

void parse_keyword(History* history){
    assert(peek_next_token()->type == TOKEN_TYPE_KEYWORD);
    Token* token = peek_next_token();
    if(is_keyword_variable_modifier(token->value.string_val) || keyword_is_datatype(token->value.string_val)){
        parse_variable_or_function_or_struct_or_union(history);
    }
}

static bool is_keyword_variable_modifier(const char* value){
    return ARE_STRINGS_EQUAL(value, "unsigned") || ARE_STRINGS_EQUAL(value, "signed") || ARE_STRINGS_EQUAL(value, "static") || ARE_STRINGS_EQUAL(value, "const") || ARE_STRINGS_EQUAL(value, "extern") || ARE_STRINGS_EQUAL(value, "__ignore_typecheck__");
}

bool keyword_is_datatype(const char* value){
    return ARE_STRINGS_EQUAL(value, "int") || ARE_STRINGS_EQUAL(value, "char") || ARE_STRINGS_EQUAL(value, "float") || ARE_STRINGS_EQUAL(value, "double") || ARE_STRINGS_EQUAL(value, "void") || ARE_STRINGS_EQUAL(value, "long") || ARE_STRINGS_EQUAL(value, "short") || ARE_STRINGS_EQUAL(value, "struct") || ARE_STRINGS_EQUAL(value, "union");
}

void parse_variable_or_function_or_struct_or_union(History* history){
    DataType datatype;
    parse_datatype(&datatype);
    if(is_datatype_struct_or_union(&datatype) && is_token_symbol(peek_next_token(), '{')){
        parse_struct_or_union(&datatype);
        Node* struct_or_union_node = pop_node();
        symbol_resolver_build_for_node(current_process, struct_or_union_node);
        push_node(struct_or_union_node);
        return;
    }
    parser_ignore_int(&datatype);
    Token* name_token = get_next_token();
    if(!name_token){
        return;
    }
    if(name_token->type != TOKEN_TYPE_IDENTIFIER){
        compiler_error(current_process, "expecting a valid name for variable or function\n");
    }
    parse_variable(&datatype, name_token, history);
    if(is_next_token_operator(",")){
        DynamicVector* variable_list = create_vector(sizeof(Node*));
        Node* variable_node = pop_node();
        push_element(variable_list, &variable_node);
        while(is_next_token_operator(",")){
            get_next_token();
            name_token = get_next_token();
            parse_variable(&datatype, name_token, history);
            variable_node = pop_node();
            push_element(variable_list, &variable_node);
        }
        make_variable_list_node(variable_list);
    }
    expect_symbol(';');
}

void parse_datatype(DataType* datatype){
    memset(datatype, 0, sizeof(DataType));
    datatype->flags |= DATATYPE_FLAG_IS_SIGNED;
    parse_datatype_modifier(datatype);
    parse_datatype_type(datatype);
    parse_datatype_modifier(datatype);
}

void parse_datatype_modifier(DataType* data_type){
    Token* token = peek_next_token();
    while(token && token->type == TOKEN_TYPE_KEYWORD){
        if(!is_keyword_variable_modifier(token->value.string_val)){
            break;
        }
        if(ARE_STRINGS_EQUAL(token->value.string_val, "unsigned")){
            data_type->flags &= ~DATATYPE_FLAG_IS_SIGNED;
        }
        else if(ARE_STRINGS_EQUAL(token->value.string_val, "signed")){
            data_type->flags |= DATATYPE_FLAG_IS_SIGNED;
        }
        else if(ARE_STRINGS_EQUAL(token->value.string_val, "static")){
            data_type->flags |= DATATYPE_FLAG_IS_STATIC;
        }
        else if(ARE_STRINGS_EQUAL(token->value.string_val, "const")){
            data_type->flags |= DATATYPE_FLAG_IS_CONST;
        }
        else if(ARE_STRINGS_EQUAL(token->value.string_val, "extern")){
            data_type->flags |= DATATYPE_FLAG_IS_EXTERN;
        }
        else if(ARE_STRINGS_EQUAL(token->value.string_val, "__ignore_typecheck__")){
            data_type->flags |= DATATYPE_FLAG_IGNORE_TYPE_CHECK;
        }
        else{
            compiler_error(current_process, "unknown datatype modifier");
        }
        get_next_token();
        token = peek_next_token();
    }
}

void parse_datatype_type(DataType* data_type){
    Token* datatype_token = NULL;
    Token* datatype_token_secondary = NULL;
    parser_get_datatype_tokens(&datatype_token, &datatype_token_secondary);
    int expected_type = parser_datatype_expected_for_type_string(datatype_token->value.string_val);
    if(is_datatype_struct_or_union_given_name(datatype_token->value.string_val)){
        if(peek_next_token()->type == TOKEN_TYPE_IDENTIFIER){
            datatype_token  = get_next_token();
        }
        else{
            //this structure has no name, so handle it
            datatype_token = parser_assign_random_name_to_struct_or_union(datatype_token);
            data_type->flags |= DATATYPE_FLAG_STRUCT_OR_UNION_NO_NAME;
        }
    }

    int pointer_level = parser_get_pointer_level();
    parser_datatype_init(datatype_token, datatype_token_secondary, data_type, pointer_level, expected_type);
}

void parser_get_datatype_tokens(Token** datatype_token_out, Token** datatype_token_secondary_out){
    *datatype_token_out = get_next_token();
    Token* next_token = peek_next_token();
    if(is_token_primitive_keyword(next_token)){
        *datatype_token_secondary_out = next_token;
        get_next_token();
    }
}

int parser_datatype_expected_for_type_string(const char* value){
    int type = DATA_TYPE_EXPECT_PRIMITIVE;
    if(ARE_STRINGS_EQUAL(value, "struct")){
        type = DATA_TYPE_EXPECT_STRUCT;
    }
    else if(ARE_STRINGS_EQUAL(value, "union")){
        type = DATA_TYPE_EXPECT_UNION;
    }
    return type;
}

Token* parser_assign_random_name_to_struct_or_union(){
    char temporary_name[25];
    sprintf(temporary_name, "customtypename_%i", parser_get_random_type_index());
    char* name = malloc(strlen(temporary_name) + 1);
    strcpy(name, temporary_name);
    Token* token = calloc(1, sizeof(Token));
    token->type = TOKEN_TYPE_IDENTIFIER;
    token->value.string_val = name;
    return token;
}

int parser_get_random_type_index(){
    static int index = 0;
    return index++;
}

int parser_get_pointer_level(){
    int depth = 0;
    while(is_next_token_operator("*")){
        get_next_token();
        depth++;
    }
    return depth;
}

bool is_next_token_operator(const char* operator){
    Token* token = peek_next_token();
    return token && token->type == TOKEN_TYPE_OPERATOR && ARE_STRINGS_EQUAL(token->value.string_val, operator);
}

void parser_datatype_init(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out, int pointer_depth, int expected_type){
    parser_datatype_init_type_and_size(datatype_token, datatype_secondary_token, datatype_out, pointer_depth, expected_type);
    datatype_out->name = datatype_token->value.string_val;
    if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "long") && datatype_secondary_token && ARE_STRINGS_EQUAL(datatype_secondary_token->value.string_val, "long")){
        compiler_warning(current_process, "compiler doesn't suppport 64 bit longs, using 32 bit longs instead");
        datatype_out->size = DATA_SIZE_DWORD;
    }
}

void parser_datatype_init_type_and_size(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out, int pointer_depth, int expected_type){
    if(!is_secondary_datatype_allowed(expected_type) && datatype_secondary_token){
        compiler_error(current_process, "secondary datatype not allowed");
    }
    switch(expected_type){
        case DATA_TYPE_EXPECT_PRIMITIVE:
            parser_datatype_init_type_and_size_for_primitive(datatype_token, datatype_secondary_token, datatype_out);
            break;
        case DATA_TYPE_EXPECT_STRUCT:
            datatype_out->type = DATA_TYPE_STRUCT;
            datatype_out->size = get_size_of_struct(datatype_token->value.string_val);
            datatype_out->data.struct_node = get_struct_node_for_name(current_process, datatype_token->value.string_val);
            break;
        case DATA_TYPE_EXPECT_UNION:
            compiler_error(current_process, "struct or union are currently not supported");
            break;
        default:
            compiler_error(current_process, "BUG: unknown expected type");
    }
}

bool is_secondary_datatype_allowed(int expected_type){
    return expected_type == DATA_TYPE_EXPECT_PRIMITIVE;
}

bool is_secondary_datatype_allowed_for_type(const char* expected_type){
    return ARE_STRINGS_EQUAL(expected_type, "float") || ARE_STRINGS_EQUAL(expected_type, "double") || ARE_STRINGS_EQUAL(expected_type, "long") || ARE_STRINGS_EQUAL(expected_type, "short");
}

void parser_datatype_init_type_and_size_for_primitive(Token* datatype_token, Token* datatype_secondary_token, DataType* datatype_out){
    if(!is_secondary_datatype_allowed_for_type(datatype_token->value.string_val) && datatype_secondary_token){
        compiler_error(current_process, "secondary datatype not allowed");
    }
    if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "void")){
        datatype_out->type = DATA_TYPE_VOID;
        datatype_out->size = DATA_SIZE_ZERO;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "int")){
        datatype_out->type = DATA_TYPE_INT;
        datatype_out->size = DATA_SIZE_DWORD;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "char")){
        datatype_out->type = DATA_TYPE_CHAR;
        datatype_out->size = DATA_SIZE_BYTE;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "float")){
        datatype_out->type = DATA_TYPE_FLOAT;
        datatype_out->size = DATA_SIZE_DWORD;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "double")){
        datatype_out->type = DATA_TYPE_DOUBLE;
        datatype_out->size = DATA_SIZE_DWORD;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "long")){
        datatype_out->type = DATA_TYPE_LONG;
        datatype_out->size = DATA_SIZE_DWORD;
    }
    else if(ARE_STRINGS_EQUAL(datatype_token->value.string_val, "short")){
        datatype_out->type = DATA_TYPE_SHORT;
        datatype_out->size = DATA_SIZE_WORD;
    }
    else{
        compiler_error(current_process, "unknown primitive datatype");
    }
    parser_adjust_size_for_secondary_datatype(datatype_out, datatype_secondary_token);
}

void parser_adjust_size_for_secondary_datatype(DataType* datatype, Token* datatype_secondary_token){
    if(!datatype_secondary_token){
        return;
    }
    DataType* secondary_data_type = calloc(1, sizeof(DataType));
    parser_datatype_init_type_and_size_for_primitive(datatype_secondary_token, NULL, secondary_data_type);
    datatype->size+= secondary_data_type->size;
    datatype->secondary_data_type = secondary_data_type;
    datatype->flags |= DATATYPE_FLAG_IS_SECONDARY;
}

void parse_keyword_for_global(){
    parse_keyword(begin_history(0));
    Node* node = pop_node();
    //later do something with the node
    push_node(node);
}

void parser_ignore_int(DataType* datatype){
    if(!is_token_keyword(peek_next_token(), "int")){
        return;
    }
    if(!parser_is_init_valid_after_datatype(datatype)){
        compiler_error(current_process, "provided secondary datatype int is not valid");
    }
    get_next_token();
}

bool parser_is_init_valid_after_datatype(DataType* datatype){
    return datatype->type == DATA_TYPE_LONG || datatype->type == DATA_TYPE_FLOAT || datatype->type == DATA_TYPE_DOUBLE;
}

void parse_variable(DataType* datatype, Token* name_token, History* history){
    #warning "check for array brackets"
    Node* value_node = NULL;
    ArrayBrackets* array_brackets = NULL;
    if(is_next_token_operator("[")){
        array_brackets = parse_array_brackets(history);
        datatype->flags |= DATATYPE_FLAG_IS_ARRAY;
        datatype->array.array_bracket = array_brackets;
        datatype->array.size = array_brackets_calculate_size(datatype, array_brackets);
    }
    if(is_next_token_operator("=")){
        get_next_token();
        parse_expressionable_root(history);
        value_node = pop_node();
    }
    make_variable_node_and_register(history, datatype, name_token, value_node);
}

void parse_expressionable_root(History* history){
    parse_expressionable(history);
    Node* result_node = pop_node();
    push_node(result_node);
}

void make_variable_node_and_register(History* history, DataType* datatype, Token* name_token, Node* value_node){
    make_variable_node(datatype, name_token, value_node);
    Node* variable_node = pop_node();
    #warning "register the variable, i.e., calculate scope offset and push the variable node to scope"
    //calculate scope offset
    parser_scope_offset_calculate(history, variable_node);
    //push variable node to scope
    push_parser_scope(create_new_parser_scope_entity(variable_node, variable_node->data.var.aligned_offset, 0), variable_node->data.var.data_type.size);
    push_node(variable_node);
}

void make_variable_node(DataType* datatype, Token* name_token, Node* value_node){
    const char* name_string = NULL;
    if(name_token){
        name_string = name_token->value.string_val;
    }
    create_node(&((Node){.type = NODE_TYPE_VARIABLE, .data.var.data_type = *datatype, .data.var.name = name_string, .data.var.value = value_node}));
}

void make_variable_list_node(DynamicVector* variable_list){
    create_node(&((Node){.type = NODE_TYPE_VARIABLE_LIST, .data.variable_list.variables = variable_list}));
}

static void expect_symbol(const char symbol){
    Token* next_token = get_next_token();
    if(!next_token || next_token->type != TOKEN_TYPE_SYMBOL || next_token->value.char_val != symbol){
        compiler_error(current_process, "expecting symbol %c", symbol);
    }
}

ArrayBrackets* parse_array_brackets(History* history){
    ArrayBrackets* array_brackets = array_brackets_new(1);
    while(is_next_token_operator("[")){
        expect_operator("[");
        if(is_token_symbol(peek_next_token(), ']')){
            expect_symbol(']');
            break;
        }
        parse_expressionable_root(history);
        expect_symbol(']');
        Node* expression_node = pop_node();
        make_bracket_node(expression_node);
        Node* bracket_node = pop_node();
        add_array_bracket(array_brackets, bracket_node);
    }
    return array_brackets;
}

static void expect_operator(const char* operator){
    Token* next_token = get_next_token();
    if(!next_token || next_token->type != TOKEN_TYPE_OPERATOR || !ARE_STRINGS_EQUAL(next_token->value.string_val, operator)){
        compiler_error(current_process, "expecting operator %s", operator);
    }
}

void make_bracket_node(Node* expression_node){
    create_node(&((Node){.type = NODE_TYPE_BRACKET, .data.bracket.inner = expression_node}));
}

static bool is_next_token_symbol(const char symbol){
    Token* token = peek_next_token();
    return is_token_symbol(token, symbol);
}

void parse_struct_or_union(DataType* datatype){
    switch(datatype->type){
        case DATA_TYPE_STRUCT:
            parse_struct(datatype);
            break;
        case DATA_TYPE_UNION:
            parse_union(datatype);
            break;
        default:
            compiler_error(current_process, "COMPLIER BUG: unknown struct or union type");
    }
}

void parse_struct(DataType* datatype){
    bool is_forward_declaration = !is_token_symbol(peek_next_token(), '{');
    if(!is_forward_declaration){
        parser_new_scope();
    }
    parse_struct_no_new_scope(datatype, is_forward_declaration);
    if(!is_forward_declaration){
        parser_finish_scope();
    }
}

void parser_new_scope(){
    new_scope(current_process, 0);
}

enum{
    HISTORY_FLAG_INSIDE_UNION = 0b00000001,
    HISTORY_FLAG_IS_UPWARD_STACK = 0b00000010,
    HISTORY_FLAG_IS_GLOBAL_SCOPE = 0b00000100,
    HISTORY_FLAG_INSIDE_STRUCTURE = 0b00001000,
};


void parse_struct_no_new_scope(DataType* datatype, bool is_foward_declaration){
    Node* body_node = NULL;
    size_t body_variable_size = 0;
    if(!is_foward_declaration){
        parse_body(&body_variable_size, begin_history(HISTORY_FLAG_INSIDE_STRUCTURE));
        body_node = pop_node();
    }
    make_struct_node(datatype->name, body_node);
    Node* struct_node = pop_node();
    if(body_node){
        datatype->size = body_node->data.body.size;
    }
    datatype->data.struct_node = struct_node;
    if(peek_next_token()->type == TOKEN_TYPE_IDENTIFIER){
        Token* variable_name = get_next_token();
        struct_node->flags |= NODE_FLAG_HAS_VARIABLE_COMBINED;
        if(datatype->flags & DATATYPE_FLAG_STRUCT_OR_UNION_NO_NAME){
            datatype->name = variable_name->value.string_val;
            datatype->flags &= ~DATATYPE_FLAG_STRUCT_OR_UNION_NO_NAME;
            struct_node->data.structure.name = variable_name->value.string_val;
        }
        make_variable_node_and_register(begin_history(HISTORY_FLAG_INSIDE_STRUCTURE), datatype, variable_name, NULL);
        struct_node->data.structure.variable = pop_node();
    }
    expect_symbol(';');
    push_node(struct_node);
}

void parser_finish_scope(){
    finish_scope(current_process);
}

void parse_body(size_t* sum_of_var_size, History* history){
    parser_new_scope();
    size_t temp_size = 0x00;
    if(!sum_of_var_size){
        sum_of_var_size = &temp_size;
    }
    DynamicVector* body_vector = create_vector(sizeof(Node*));
    if(!is_next_token_symbol('{')){
        parse_body_single_statement(sum_of_var_size, body_vector, history);
        parser_finish_scope();
    }
    parse_body_multiple_statements(sum_of_var_size, body_vector, history);
    parser_finish_scope();
    #warning "don't forget to adjust function stack size"
}

// extern Node* parser_current_body_node;

void parse_body_single_statement(size_t* sum_of_var_size, DynamicVector* body_vector, History* history){
    make_body_node(NULL, 0, false, NULL);
    Node* body_node = pop_node();
    body_node->BindedTo.body = parser_current_body_node;
    parser_current_body_node = body_node;
    Node* statement_node = NULL;
    parse_statement(history);
    statement_node = pop_node();
    push_element(body_vector, &statement_node);
    parser_apppend_size_for_node(history, sum_of_var_size, statement_node);
    Node* larget_var_node = NULL;
    if(statement_node->type == NODE_TYPE_VARIABLE){
        larget_var_node = statement_node;
    }
    parser_finalize_body(history, body_node, body_vector, sum_of_var_size, larget_var_node, larget_var_node);
    parser_current_body_node = body_node->BindedTo.body;
    push_node(body_node);
}

void make_body_node(DynamicVector* body_vector, size_t sum_of_var_size, bool padded, Node* largest_var_node){
    create_node(&((Node){.type = NODE_TYPE_BODY, .data.body.statements = body_vector, .data.body.size = sum_of_var_size, .data.body.largest_var_node = largest_var_node, .data.body.padded = padded}));
}

void parse_statement(History* history){
    if(peek_next_token()->type == TOKEN_TYPE_KEYWORD){
        parse_keyword(history);
        return;
    }
    parse_expressionable_root(history);
    if(peek_next_token()->type == TOKEN_TYPE_SYMBOL && !is_token_symbol(peek_next_token(), ';')){
        parse_symbol();
        return;
    }
    expect_symbol(';');
}


void parse_symbol(){
    if(is_next_token_symbol('{')){
        size_t variable_size = 0;
        parse_body(&variable_size, begin_history(HISTORY_FLAG_IS_GLOBAL_SCOPE));
        Node* body_node = pop_node();
        push_node(body_node);
    }
}

void parser_apppend_size_for_node(History* history, size_t* variable_size, Node* node){
    if(!node){
        return;
    }
    if(node->type == NODE_TYPE_VARIABLE){
        if(is_node_struct_or_union_variable(node)){
            parser_apppend_size_for_node_struct_or_union(history, variable_size, node);
            return;
        }
        *variable_size+= get_variable_size(node);
    }
    else if(node->type == NODE_TYPE_VARIABLE_LIST){
        parser_apppend_size_for_variable_list(history, variable_size, node->data.variable_list.variables);
    }
}

void parser_finalize_body(History* history, Node* body_node, DynamicVector* body_vector, size_t* sum_of_var_size, Node* largest_align_eligible_var_node, Node* largest_possible_var_node){
    if(history->flags & HISTORY_FLAG_INSIDE_UNION){
        //union size is the size of the largest variable
        if(largest_possible_var_node){
            *sum_of_var_size = get_variable_size(largest_possible_var_node);
        }
    }
    int padding = get_compute_sum_padding(body_vector);
    *sum_of_var_size+= padding;
    if(largest_align_eligible_var_node){
        *sum_of_var_size = get_align_value(*sum_of_var_size, largest_align_eligible_var_node->data.var.data_type.size);
    }
    bool padded = padding != 0;
    body_node->data.body.largest_var_node = largest_align_eligible_var_node;
    body_node->data.body.size = *sum_of_var_size;
    body_node->data.body.statements = body_vector;
    body_node->data.body.padded = padded;
    //compute correct padding

}

void parse_union(DataType* datatype){
    compiler_error(current_process, "union not implemented");
}

void parser_apppend_size_for_node_struct_or_union(History* history, size_t* variable_size, Node* node){
    *variable_size+= get_variable_size(node);
    if(node->data.var.data_type.flags & DATATYPE_FLAG_IS_POINTER ){
        return;
    }
    Node* largest_var_node = get_variable_struct_or_union_body_node(node)->data.body.largest_var_node;
    if(largest_var_node){
        *variable_size+= get_align_value(*variable_size, largest_var_node->data.var.data_type.size);
    }
}

void parser_apppend_size_for_variable_list(History* history, size_t* variable_size, DynamicVector* variable_list){
    set_peek_index(variable_list, 0);
    Node* variable_node = peek_pointer(variable_list);
    while(variable_node){
        parser_apppend_size_for_node(history, variable_size, variable_node);
        variable_node = peek_pointer(variable_list);
    }
}

enum{
    PARSER_SCOPE_ENTITY_ON_STACK = 0b00000001,
    PARSER_SCOPE_ENTITY_STRUCTURE_SCOPE = 0b00000010,

};

typedef struct ParserScopeEntity{
    int flags;
    int stack_offset; //-ve for local variables, +ve for global variables
    Node* variable_node;
}ParserScopeEntity;

ParserScopeEntity* create_new_parser_scope_entity(Node* variable_node, int stack_offset, int flags){
    ParserScopeEntity* entity = calloc(1, sizeof(ParserScopeEntity));
    entity->flags = flags;
    entity->stack_offset = stack_offset;
    entity->variable_node = variable_node;
    return entity;
}

ParserScopeEntity* get_parser_scope_last_entity_stop_global_scope(){
    return get_scope_last_entity_stop_at(current_process, current_process->scope.root);
}

void push_parser_scope(ParserScopeEntity* entity, size_t size){
    push_scope(current_process, entity, size);
}

void parser_scope_offset_calculate(History* history, Node* variable_node){
    if(history->flags & HISTORY_FLAG_IS_GLOBAL_SCOPE){
        parser_scope_offset_calculate_for_global(history, variable_node);
        return;
    }
    if(history->flags & HISTORY_FLAG_INSIDE_STRUCTURE){
        parser_scope_offset_calculate_for_struct(history, variable_node);
        return;
    }
    parser_scope_offset_calculate_for_stack(history, variable_node);
}

void parser_scope_offset_calculate_for_stack(History* history, Node* variable_node){
    ParserScopeEntity* last_entity = get_parser_scope_last_entity_stop_global_scope();
    bool upward_stack = history->flags & HISTORY_FLAG_IS_UPWARD_STACK;
    int offset = -get_variable_size(variable_node);
    if(upward_stack){
        #warning "implement upward stack"
        compiler_error(current_process, "upward stack not implemented");
    }
    if(last_entity){
        offset+= get_variable_node(last_entity->variable_node)->data.var.aligned_offset;
        if(is_variable_node_primitive(variable_node)){
            get_variable_node(variable_node)->data.var.padding = get_padding(upward_stack ? offset : -offset, variable_node->data.var.data_type.size);
        }
    }
}

void parser_scope_offset_calculate_for_global(History* history, Node* node){
}

void parser_scope_offset_calculate_for_struct(History* history, Node* node){
    int offset = 0;
    ParserScopeEntity* last_entity = get_parser_scope_last_entity();
    if(last_entity){
        offset += last_entity->stack_offset + last_entity->variable_node->data.var.data_type.size;
        if(is_variable_node_primitive(node)){
            node->data.var.padding = get_padding(offset, node->data.var.data_type.size);
        }
        node->data.var.aligned_offset = offset + node->data.var.padding;
    }
}

ParserScopeEntity* get_parser_scope_last_entity(){
    return get_scope_last_entity(current_process);
}

void parse_body_multiple_statements(size_t* sum_of_var_size, DynamicVector* body_vector, History* history){
    make_body_node(NULL, 0, false, NULL);
    Node* body_node = pop_node();
    body_node->BindedTo.body = parser_current_body_node;
    parser_current_body_node = body_node;
    Node* statement_node = NULL;
    Node* largest_possible_var_node = NULL;
    Node* largest_align_eligible_var_node = NULL;
    expect_symbol('{');
    while(!is_next_token_symbol('}')){
        parse_statement(clone_history(history, history->flags));
        statement_node = pop_node();
        if(statement_node->type == NODE_TYPE_VARIABLE){
            if(largest_possible_var_node == NULL || largest_possible_var_node->data.var.data_type.size <= statement_node->data.var.data_type.size){
                largest_possible_var_node = statement_node;
            }
            if(is_variable_node_primitive(statement_node)){
                if(largest_align_eligible_var_node == NULL || largest_align_eligible_var_node->data.var.data_type.size <= statement_node->data.var.data_type.size){
                    largest_align_eligible_var_node = statement_node;
                }
            }
        }
        push_element(body_vector, &statement_node);
        parser_apppend_size_for_node(history, sum_of_var_size, get_variable_node_or_list(statement_node));
    }
    expect_symbol('}');
    parser_finalize_body(history, body_node, body_vector, sum_of_var_size, largest_align_eligible_var_node, largest_possible_var_node);
    parser_current_body_node = body_node->BindedTo.body;
    push_node(body_node);
}

size_t get_size_of_struct(const char* name){
    Symbol* symbol = symbol_resolver_get_symbol(current_process, name);
    if(!symbol){
        return 0;
    }
    assert(symbol->type == SYMBOL_TYPE_NODE);
    Node* node = symbol->data;
    assert(node->type == NODE_TYPE_STRUCT);
    return node->data.body.size;
}