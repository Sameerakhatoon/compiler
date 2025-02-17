#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>

ArrayBrackets* array_brackets_new(int size) {
    ArrayBrackets* array_brackets = calloc(1, sizeof(ArrayBrackets));
    array_brackets->n_brackets = create_vector(sizeof(Node*));
    return array_brackets;
}

void free_array_brackets(ArrayBrackets* array_brackets) {
    destroy_vector(array_brackets->n_brackets);
    free(array_brackets);
}

void add_array_bracket(ArrayBrackets* array_brackets, Node* bracket_node) {
    assert(bracket_node->type == NODE_TYPE_BRACKET);
    push_element(array_brackets->n_brackets, &bracket_node);
}

DynamicVector* get_array_brackets_node_vector(ArrayBrackets* array_brackets) {
    return array_brackets->n_brackets;
}

size_t array_brackets_calculate_size_from_index(DataType* datatype, ArrayBrackets* array_brackets, int index) {
    DynamicVector* array_vector = get_array_brackets_node_vector(array_brackets);
    size_t size = datatype->size;
    if(index >= get_element_count(array_vector)) {
        return size;
    }
    set_peek_index(array_vector, index);
    Node* bracket_node = peek_pointer(array_vector);
    if(!bracket_node) {
        return size;
    }
    while(bracket_node) {
        assert(bracket_node->data.bracket.inner->type == NODE_TYPE_NUMBER);
        int number = bracket_node->data.bracket.inner->literal_value.long_long_num;
        size *= number;
        bracket_node = peek_pointer(array_vector);
    }
    return size;
}

size_t array_brackets_calculate_size(DataType* datatype, ArrayBrackets* array_brackets) {
    return array_brackets_calculate_size_from_index(datatype, array_brackets, 0);
}

int array_brackets_get_total_indices(DataType* datatype) {
    assert(datatype->flags & DATATYPE_FLAG_IS_ARRAY);
    ArrayBrackets* array_brackets = datatype->array.array_bracket;
    return get_element_count(array_brackets->n_brackets);
}
