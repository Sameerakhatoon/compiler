#include "compiler.h"
#include <assert.h>
#include "helpers/vector.h"

DynamicVector* node_vector = NULL;
DynamicVector* node_vector_root = NULL;

void set_node_vectors(DynamicVector* vector, DynamicVector* root);
void push_node(Node* node);
Node* peek_node_or_null();
Node* peek_node();
Node* pop_node();
void print_node(Node* node, int depth);
Node* peek_node_expressionable_or_null();
bool is_node_expressionable(Node* node);
void make_expression_node(Node* left, Node* right, const char* operator);

void set_node_vectors(DynamicVector* vector, DynamicVector* root) {
    node_vector = vector;
    node_vector_root = root;
}

void push_node(Node* node) {
    push_element(node_vector, &node);
}

Node* peek_node_or_null() {
    return get_last_element_pointer_or_null(node_vector);
}

Node* peek_node() {
    return get_last_element_pointer(node_vector);
}

Node* pop_node() {
    Node* last_node = get_last_element_pointer(node_vector);
    Node* last_node_root = is_vector_empty(node_vector_root) ? NULL : get_last_element_pointer_or_null(node_vector_root);
    remove_last_element(node_vector);
    if(last_node_root == last_node) { //so that we don't leave duplicates on the tree vector
        remove_last_element(node_vector_root);
    }
    return last_node;
}

void print_tabs(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
}

void print_node(Node* node, int depth) {
    if (!node) return;

    print_tabs(depth);
    
    switch (node->type) {
        case NODE_TYPE_NUMBER:
            printf("Node number: %lld\n", node->literal_value.long_long_num);
            break;
        case NODE_TYPE_STRING:
            printf("Node string: %s\n", node->literal_value.string_val);
            break;
        case NODE_TYPE_IDENTIFIER:
            printf("Node identifier: %s\n", node->literal_value.string_val);
            break;
        case NODE_TYPE_EXPRESSION:
            printf("Node expression: %s\n", node->data.expression.operator);
            
            print_tabs(depth + 1);
            printf("Left of operator %s\n", node->data.expression.operator);
            print_node(node->data.expression.left, depth + 2);

            print_tabs(depth + 1);
            printf("Right of operator %s\n", node->data.expression.operator);
            print_node(node->data.expression.right, depth + 2);
            break;
        case NODE_TYPE_VARIABLE:
            //check if variable is array
            if(node->data.var.data_type.flags & DATATYPE_FLAG_IS_ARRAY) {
                printf("Node variable: %s (array) of size %li \n", node->data.var.name, array_brackets_calculate_size(&node->data.var.data_type, node->data.var.data_type.array.array_bracket));
                for(int i = 0; i < node->data.var.data_type.array.array_bracket->n_brackets->element_count; i++) {
                    print_tabs(depth + 1);
                    print_node(*(Node**)get_element_at(node->data.var.data_type.array.array_bracket->n_brackets, i), depth + 2);
                }
            } else {
                printf("Node variable: %s\n", node->data.var.name);
                print_node(node->data.var.value, depth + 1);            }
            break;
        case NODE_TYPE_VARIABLE_LIST:
            printf("Node variable list\n");
            for (int i = 0; i < node->data.variable_list.variables->element_count; i++) {
                print_node(*(Node**)get_element_at(node->data.variable_list.variables, i), depth + 1);
            }
            break;
        case NODE_TYPE_BODY:
            printf("Node body\n");
            for (int i = 0; i < node->data.body.statements->element_count; i++) {
                print_node(*(Node**)get_element_at(node->data.body.statements, i), depth + 1);
            }
            break;
        default:
            printf("Unknown node type\n");
            printf("Node type: %i\n", node->type);
    }
}

Node* peek_node_expressionable_or_null() {
    Node* last_node = peek_node_or_null();
    if(last_node && is_node_expressionable(last_node)) {
        return last_node;
    }
    return NULL;
}

bool is_node_expressionable(Node* node) {
    return node && (node->type == NODE_TYPE_EXPRESSION || node->type == NODE_TYPE_EXPRESSION_PARENTHESES || node->type == NODE_TYPE_UNARY ||node->type == NODE_TYPE_IDENTIFIER || node->type == NODE_TYPE_NUMBER || node->type == NODE_TYPE_STRING);
}

void make_expression_node(Node* left, Node* right, const char* operator) {
    assert(left);
    assert(right);
    create_node(&((Node){.type = NODE_TYPE_EXPRESSION, .data.expression.left = left, .data.expression.right = right, .data.expression.operator = operator}));
}

Node* parser_current_body_node = NULL;

bool is_node_struct_or_union_variable(Node* node){
    if(node->type != NODE_TYPE_VARIABLE){
        return false;
    }
    return is_datatype_struct_or_union(&node->data.var.data_type);
}

Node* get_variable_node(Node* node){
    Node* variable_node = NULL;
    switch(node->type){
        case NODE_TYPE_VARIABLE:
            variable_node = node;
            break;
        case NODE_TYPE_STRUCT:
            variable_node = node->data.structure.variable;
            break;
        case NODE_TYPE_UNION:
            // variable_node = node->data.union.variable;
            assert(1==0 && "union not implemented\n");
            break;
    }
    return variable_node;
}

bool is_variable_node_primitive(Node* node){
    assert(node->type == NODE_TYPE_VARIABLE);
    return is_datatype_primitive(&node->data.var.data_type);
}

Node* get_variable_node_or_list(Node* node){
    if(node->type == NODE_TYPE_VARIABLE_LIST){
        return node;
    }
    return get_variable_node(node);
}

void make_struct_node(const char* name, Node* body_node){
    int flags = 0;
    if(!body_node){
        flags |= NODE_FLAG_IS_FORWARD_DECLARATION;
    }
    create_node(&((Node){.type = NODE_TYPE_STRUCT, .data.structure.name = name, .data.structure.body_node = body_node, .flags = flags}));
}

Node* get_struct_node_for_name(CompileProcess* process, const char* name){
    Node* node = get_node_from_symbol(process, name);
    if(!node){
        return NULL;
    }
    if(node->type != NODE_TYPE_STRUCT){
        return NULL;
    }
    return node;
}

Node* get_node_from_symbol(CompileProcess* process,const char* name){
    Symbol* symbol = symbol_resolver_get_symbol(process, name);
    if(!symbol){
        return NULL;
    }
    return get_node_form_a_symbol(symbol);
}

Node* get_node_form_a_symbol(Symbol* symbol){
    if(symbol->type != SYMBOL_TYPE_NODE){
        return NULL;
    }
    return symbol->data;
}