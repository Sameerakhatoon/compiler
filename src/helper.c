#include "compiler.h"
#include <assert.h>
#include "helpers/vector.h"

size_t get_variable_size(Node* variable_node) {
    assert(variable_node->type == NODE_TYPE_VARIABLE);
    return get_datatype_size(&variable_node->data.var.data_type);
}

size_t get_variable_size_for_list(Node* variable_list_node) {
    assert(variable_list_node->type == NODE_TYPE_VARIABLE_LIST);
    size_t size = 0;
    set_peek_index(variable_list_node->data.variable_list.variables, 0);
    Node* variable_node = peek_pointer(variable_list_node->data.variable_list.variables);
    while(variable_node != NULL) {
        size += get_variable_size(variable_node);
        variable_node = peek_pointer(variable_list_node->data.variable_list.variables);
    }
    return size;
}

int get_padding(int value, int to){
    if(to<=0){
        return 0;
    }
    if(value%to==0){
        return 0;
    }
    return to - (value%to) % to;
}

int get_align_value(int val, int to){
    if(val%to){
        val+=get_padding(val, to);
    }
    return val;
}

int get_align_value_treat_positive(int val, int to){
    assert(to>=0);
    if(val<0){
        to = -to;
    }
    return get_align_value(val, to);
}

int get_compute_sum_padding(DynamicVector* vector){
    int padding = 0;
    int last_type = -1;
    bool mixed_types = false;
    set_peek_index(vector, 0);
    Node* current_node = peek_pointer(vector);
    Node* last_node = NULL;
    while(current_node){
        if(current_node->type != NODE_TYPE_VARIABLE){
            current_node = peek_pointer(vector);
            continue;
        }
        padding += current_node->data.var.padding;
        last_type = current_node->data.var.data_type.type;
        last_node = current_node;
        current_node = peek_pointer(vector);
    }
}

Node* get_variable_struct_or_union_body_node(Node* node){
    if(!is_node_struct_or_union_variable(node)){
        return NULL;
    }
    if(node->data.var.data_type.type == DATA_TYPE_STRUCT){
        return node->data.var.data_type.data.struct_node->data.structure.body_node;
    }
    //return union body node
    #warning "union body node not implemented"
    printf("union body node not implemented\n");
    exit(1);
}