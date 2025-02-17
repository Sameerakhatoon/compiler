#include "vector.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "../compiler.h"


DynamicVector* create_vector(size_t element_size);
DynamicVector* create_vector_no_saved_states(size_t element_size);
void destroy_vector(DynamicVector* vector);
void* get_element_at(DynamicVector* vector, int index);
void* peek_element_pointer_at(DynamicVector* vector, int index);
void* peek_element_without_increment(DynamicVector* vector);
static bool is_vector_in_bounds_for_at(DynamicVector* vector, int index);
void* peek_element(DynamicVector* vector);
void* peek_element_by_index(DynamicVector* vector, int index);
void set_vector_flag(DynamicVector* vector, int flag);
void unset_vector_flag(DynamicVector* vector, int flag);
void* peek_pointer(DynamicVector* vector);
void set_peek_index(DynamicVector* vector, int index);
void set_peek_index_to_end(DynamicVector* vector);
void push_element(DynamicVector* vector, void* element);
void insert_element_at(DynamicVector* vector, int index, void* element);
void remove_last_element(DynamicVector* vector);
void remove_last_peeked_element(DynamicVector* vector);
void remove_element_at(DynamicVector* vector, int index);
static void* get_vector_data_end_pointer(DynamicVector* vector);
void* get_last_element(DynamicVector* vector);
static void assert_bounds_for_pop(DynamicVector* vector, int index);
void* get_last_element_or_null(DynamicVector* vector);
void* get_last_element_pointer_or_null(DynamicVector* vector);
void* get_last_element_pointer(DynamicVector* vector);
void* get_last_element_pointer_or_null(DynamicVector* vector);
const char* convert_vector_to_string(DynamicVector* vector);
bool is_vector_empty(DynamicVector* vector);
void clear_vector(DynamicVector* vector);
int get_element_count(DynamicVector* vector);
int read_vector_from_file(DynamicVector* vector, int element_count, FILE* file_pointer);
void* get_vector_data_pointer(DynamicVector* vector);
int insert_vector_at(DynamicVector* destination_vector, DynamicVector* source_vector, int destination_index);
int remove_element_by_data_address(DynamicVector* vector, void* element_address);
int remove_element_by_value(DynamicVector* vector, void* value);
void remove_element_by_index(DynamicVector* vector, int index);
void* get_vector_data_end(DynamicVector* vector);
void move_peek_pointer_backward(DynamicVector* vector);
int get_next_insert_index(DynamicVector* vector);
void save_vector_state(DynamicVector* vector);
void restore_vector_state(DynamicVector* vector);
void discard_last_saved_state(DynamicVector* vector);
size_t get_vector_element_size(DynamicVector* vector);
DynamicVector* clone_vector(DynamicVector* vector);




DynamicVector* create_vector(size_t element_size)
{
    DynamicVector* vector = create_vector_no_saved_states(element_size);
    vector->saved_states = create_vector_no_saved_states(sizeof(DynamicVector));
    return vector;
}

// Creates a dynamic vector without saved states, allocating memory for storage and initializing its metadata.
DynamicVector* create_vector_no_saved_states(size_t element_size)
{
    DynamicVector* vector = calloc(sizeof(DynamicVector), 1);
    vector->data_buffer = malloc(VECTOR_MINIMUM_EXTRA_CAPACITY * element_size);
    vector->element_size = element_size;
    vector->max_index = VECTOR_MINIMUM_EXTRA_CAPACITY - 1;
    vector->read_index = 0;
    vector->peek_index = 0;
    vector->element_count = 0;
    return vector;
}

void destroy_vector(DynamicVector* vector)
{
    if (!vector) return;
    free(vector->data_buffer);
    free(vector->saved_states);
    free(vector);
}

void* get_element_at(DynamicVector* vector, int index)
{
    if (index < 0 || index >= vector->element_count) return NULL;
    return (char*)vector->data_buffer + (index * vector->element_size);
}

void* peek_element_pointer_at(DynamicVector* vector, int index)
{
    if(index < 0 || index >= vector->element_count){
        return NULL;
    }
    void** element_pointer = get_element_at(vector, index);
    if(!element_pointer){
        return NULL;
    }
    return *element_pointer;
}

void* peek_element_without_increment(DynamicVector* vector){
    if(!is_vector_in_bounds_for_at(vector, vector->peek_index)){
        return NULL;
    }
    void* element = get_element_at(vector, vector->peek_index);
    return element;
}

// Checks if the index is within the bounds of the vector
static bool is_vector_in_bounds_for_at(DynamicVector* vector, int index){
    return index >= 0 && index < vector->element_count;
}

void* peek_element(DynamicVector* vector)
{
    void* element = peek_element_without_increment(vector);
    if(!element){
        return NULL;
    }
    if(vector->flag_settings & VECTOR_FLAG_DECREMENT_PEEK){
        vector->peek_index--;
    }
    else{
        vector->peek_index++;
    }
    return element;
}

void* peek_element_by_index(DynamicVector* vector, int index) {
    if(index < 0 || index >= vector->element_count){
        return NULL;
    }
    void** element_pointer = get_element_at(vector, index);
    if(!element_pointer){
        return NULL;
    }
    return *element_pointer;
}

void set_vector_flag(DynamicVector* vector, int flag)
{
    vector->flag_settings |= flag;
}

void unset_vector_flag(DynamicVector* vector, int flag)
{
    vector->flag_settings &= ~flag;
}

void* peek_pointer(DynamicVector* vector){
    void** element_pointer = peek_element(vector);
    if(!element_pointer){
        return NULL;
    }
    return *element_pointer;
}

void set_peek_index(DynamicVector* vector, int index){
    vector->peek_index = index;
}

void set_peek_index_to_end(DynamicVector* vector){
    set_peek_index(vector, vector->element_count-1);
}

void push_element(DynamicVector* vector, void* element)
{
    if (vector->element_count >= vector->max_index)
    {
        int new_capacity = vector->max_index + VECTOR_MINIMUM_EXTRA_CAPACITY;
        void* new_memory = realloc(vector->data_buffer, new_capacity * vector->element_size);
        if (!new_memory) return;
        vector->data_buffer = new_memory;
        vector->max_index = new_capacity;
    }

    void* destination = (char*)vector->data_buffer + (vector->element_count * vector->element_size);
    memcpy(destination, element, vector->element_size);
    vector->element_count++;
    vector->read_index++;
    vector->peek_index++;
}

void insert_element_at(DynamicVector* vector, int index, void* element)
{
    if (index < 0 || index > vector->element_count) return;

    push_element(vector, NULL); // Expand capacity
    memmove((char*)vector->data_buffer + ((index + 1) * vector->element_size),
            (char*)vector->data_buffer + (index * vector->element_size),
            (vector->element_count - index - 1) * vector->element_size);
    
    memcpy(get_element_at(vector, index), element, vector->element_size);
}

void remove_last_element(DynamicVector* vector)
{
    if (vector->element_count > 0)
    {
        vector->element_count--;
        vector->read_index--;
    }
}

void remove_last_peeked_element(DynamicVector* vector) {
    assert(vector->peek_index > 0);
    remove_element_at(vector, vector->peek_index - 1);
}

void remove_element_at(DynamicVector* vector, int index)
{   
    void* destination_position = get_element_at(vector, index);
    void* position_of_next_element = destination_position + vector->element_size;
    void* end_position = get_vector_data_end_pointer(vector);
    size_t total_bytes_to_move = (size_t)end_position - (size_t)position_of_next_element;
    memcpy(destination_position, position_of_next_element, total_bytes_to_move);
    vector->element_count--;
    vector->read_index--;
    vector->peek_index--;
}

// Returns a pointer to the end of the used data in the vector by calculating the offset based on element count.  
static void* get_vector_data_end_pointer(DynamicVector* vector){
    return vector->data_buffer + (vector->element_count * vector->element_size);
}

void* get_last_element(DynamicVector* vector)
{
    assert_bounds_for_pop(vector, vector->element_count-1);
    return get_element_at(vector, vector->element_count-1);
}

// Ensures that the given index is within valid bounds before performing a pop operation.  
static void assert_bounds_for_pop(DynamicVector* vector, int index)
{
    assert(index >= 0 && index < vector->element_count);
}

void* get_last_element_or_null(DynamicVector* vector) {
    if(!is_vector_in_bounds_for_at(vector, vector->element_count - 1)){
        return NULL;
    }
    return get_element_at(vector, vector->element_count - 1);
}

void* get_last_element_pointer(DynamicVector* vector)
{
    void** last_element = get_last_element(vector);
    if (!last_element) return NULL;
    return *last_element;
}

void* get_last_element_pointer_or_null(DynamicVector* vector) {
    void ** ptr = get_last_element_or_null(vector);
    if(ptr){
        return *ptr;
    }
    return NULL;
}

const char* convert_vector_to_string(DynamicVector* vector) {
    if (!vector || vector->element_count == 0) return "[]";
    
    static char buffer[1024];
    buffer[0] = '\0';

    strcat(buffer, "[");
    for (int i = 0; i < vector->element_count; i++) {
        char temp[50];
        snprintf(temp, sizeof(temp), "%d", *(int*)get_element_at(vector, i));
        strcat(buffer, temp);
        if (i < vector->element_count - 1) strcat(buffer, ", ");
    }
    strcat(buffer, "]");

    return buffer;
}

bool is_vector_empty(DynamicVector* vector)
{
    return vector->element_count == 0;
}

void clear_vector(DynamicVector* vector)
{
    while(get_element_count(vector)){
        remove_last_element(vector);
        vector->peek_index--;
    }
}

int get_element_count(DynamicVector* vector)
{
    return vector->element_count;
}

int read_vector_from_file(DynamicVector* vector, int element_count, FILE* file_pointer)
{
    if (!vector || !file_pointer) return -1;

    size_t read_elements = fread(vector->data_buffer, vector->element_size, element_count, file_pointer);
    vector->element_count += read_elements;
    
    return read_elements;
}

void* get_vector_data_pointer(DynamicVector* vector)
{
    return vector->data_buffer;
}

int insert_vector_at(DynamicVector* destination_vector, DynamicVector* source_vector, int destination_index) {
    if (!destination_vector || !source_vector) return -1;
    if (destination_index < 0 || destination_index > destination_vector->element_count) return -1;

    for (int i = 0; i < source_vector->element_count; i++) {
        insert_element_at(destination_vector, destination_index + i, get_element_at(source_vector, i));
    }
    return 0;
}

int remove_element_by_data_address(DynamicVector* vector, void* element_address) {
    for (int i = 0; i < vector->element_count; i++) {
        if (get_element_at(vector, i) == element_address) {
            remove_element_at(vector, i);
            return i;
        }
    }
    return -1;
}

int remove_element_by_value(DynamicVector* vector, void* value)
{
    for (int index = 0; index < vector->element_count; index++)
    {
        if (memcmp(get_element_at(vector, index), value, vector->element_size) == 0)
        {
            remove_element_at(vector, index);
            return index;
        }
    }
    return -1;
}

void remove_element_by_index(DynamicVector* vector, int index){
    void* destination_position = get_element_at(vector, index);
    void* position_of_next_element = destination_position + vector->element_size;
    void* end_position = get_vector_data_end(vector);
    size_t total_bytes_to_move = (size_t)end_position - (size_t)position_of_next_element;
    memcpy(destination_position, position_of_next_element, total_bytes_to_move);
    vector->element_count--;
    vector->read_index--;
}

// Returns a pointer to the end of the valid data in the vector based on the read index.
void* get_vector_data_end(DynamicVector* vector){
    return vector->data_buffer + (vector->element_count * vector->element_size);
}

void move_peek_pointer_backward(DynamicVector* vector)
{
    if (vector->peek_index > 0)
    {
        vector->peek_index--;
    }
}

int get_next_insert_index(DynamicVector* vector) {
    return vector->element_count;
}

void save_vector_state(DynamicVector* vector)
{
    if (!vector) return;

    DynamicVector* new_state = (DynamicVector*)malloc(sizeof(DynamicVector));
    if (!new_state) return;

    *new_state = *vector; // Copy struct values
    vector->saved_states = new_state;
}

void restore_vector_state(DynamicVector* vector)
{
    if (!vector || !vector->saved_states) return;

    *vector = *vector->saved_states;
    free(vector->saved_states);
    vector->saved_states = NULL;
}

void discard_last_saved_state(DynamicVector* vector)
{
    if (vector->saved_states)
    {
        free(vector->saved_states);
        vector->saved_states = NULL;
    }
}

size_t get_vector_element_size(DynamicVector* vector){
    return vector->element_size;
}

DynamicVector* clone_vector(DynamicVector* vector)
{
    if (!vector) return NULL;

    DynamicVector* cloned_vector = create_vector(vector->element_size);
    if (!cloned_vector) return NULL;

    cloned_vector->element_count = vector->element_count;
    cloned_vector->max_index = vector->max_index;
    cloned_vector->flag_settings = vector->flag_settings;

    cloned_vector->data_buffer = malloc(vector->max_index * vector->element_size);
    if (!cloned_vector->data_buffer)
    {
        free(cloned_vector);
        return NULL;
    }

    memcpy(cloned_vector->data_buffer, vector->data_buffer, vector->element_count * vector->element_size);
    return cloned_vector;
}