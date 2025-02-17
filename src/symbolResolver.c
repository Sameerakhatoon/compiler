#include "compiler.h"
#include "helpers/vector.h"

void initialize_symbol_resolver(CompileProcess* process);

static void symbol_resolver_push_symbol(CompileProcess* process, Symbol* symbol);

void symbol_resolver_new_table(CompileProcess* process);

void symbol_resolver_pop_table(CompileProcess* process);

Symbol* symbol_resolver_get_symbol(CompileProcess* process, const char* name);

Symbol* symbol_resolver_get_symbol_for_native_function(CompileProcess* process, const char* name);

Symbol* symbol_resolver_register_symbol(CompileProcess* process, const char* name, int type, void* data);

Node* symbol_resolver_get_node(Symbol* symbol);

void symbol_resolver_for_variable_node(CompileProcess* process, Node* node);

void symbol_resolver_for_function_node(CompileProcess* process, Node* node);

void symbol_resolver_for_struct_node(CompileProcess* process, Node* node);

void symbol_resolver_for_union_node(CompileProcess* process, Node* node);

void void_symbol_resolver_build_for_node(CompileProcess* process, Node* node);





void initialize_symbol_resolver(CompileProcess* process){
    process->symbols.tables = create_vector(sizeof(DynamicVector*));
}

static void symbol_resolver_push_symbol(CompileProcess* process, Symbol* symbol){
    push_element(process->symbols.current_active_symbol_table, &symbol);
}

void symbol_resolver_new_table(CompileProcess* process){
    //save current table
    push_element(process->symbols.tables, &process->symbols.current_active_symbol_table);
    //overwrite current table
    process->symbols.current_active_symbol_table = create_vector(sizeof(Symbol*));
}

void symbol_resolver_pop_table(CompileProcess* process){
    DynamicVector* last_table = get_last_element_pointer(process->symbols.tables);
    process->symbols.current_active_symbol_table = last_table;
    remove_last_element(process->symbols.tables);
}

Symbol* symbol_resolver_get_symbol(CompileProcess* process, const char* name){
    set_peek_index(process->symbols.current_active_symbol_table, 0);
    Symbol* symbol = get_last_element_pointer(process->symbols.current_active_symbol_table);
    while(symbol){
        if(ARE_STRINGS_EQUAL(symbol->name, name)){
            break;
        }
        symbol = get_last_element_pointer(process->symbols.current_active_symbol_table);
    }
    return symbol;
}

Symbol* symbol_resolver_get_symbol_for_native_function(CompileProcess* process, const char* name){
    Symbol* symbol = symbol_resolver_get_symbol(process, name);
    if(symbol == NULL){
        return NULL;
    }
    if(symbol->type != SYMBOL_TYPE_NATIVE_FUNCTION){
        return NULL;
    }
    return symbol;
}

Symbol* symbol_resolver_register_symbol(CompileProcess* process, const char* name, int type, void* data){
    if(symbol_resolver_get_symbol(process, name)){
        return NULL;
    }
    Symbol* symbol = calloc(1, sizeof(Symbol));
    symbol->name = name;
    symbol->type = type;
    symbol->data = data;
    symbol_resolver_push_symbol(process, symbol);
    return symbol;
}

Node* symbol_resolver_get_node(Symbol* symbol){
    if(symbol->type != SYMBOL_TYPE_NODE){
        return NULL;
    }
    return symbol->data;
}

void symbol_resolver_for_variable_node(CompileProcess* process, Node* node){
    compiler_error(process, "Variable nodes are not supported yet");
}

void symbol_resolver_for_function_node(CompileProcess* process, Node* node){
    compiler_error(process, "Function nodes are not supported yet");
}

void symbol_resolver_for_struct_node(CompileProcess* process, Node* node){
    compiler_error(process, "Struct nodes are not supported yet");
}

void symbol_resolver_for_union_node(CompileProcess* process, Node* node){
    compiler_error(process, "Union nodes are not supported yet");
}

void void_symbol_resolver_build_for_node(CompileProcess* process, Node* node){
    switch(node->type){
        case NODE_TYPE_VARIABLE:
            symbol_resolver_for_variable_node(process, node);
            break;
        case NODE_TYPE_FUNCTION:
            symbol_resolver_for_function_node(process, node);
            break;
        case NODE_TYPE_STRUCT:
            symbol_resolver_for_struct_node(process, node);
            break;
        case NODE_TYPE_UNION:
            symbol_resolver_for_union_node(process, node);
            break;
        //ignore all other node types as they can't be symbols
    }
}