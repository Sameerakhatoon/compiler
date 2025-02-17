/*
* @file vector.h
* @brief Header file for the DynamicVector struct and its functions
* @details Contains function prototypes for the vector.c file
*/

#ifndef DYNAMIC_VECTOR_H
#define DYNAMIC_VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


/*
* @def VECTOR_MINIMUM_EXTRA_CAPACITY
* @brief Minimum extra capacity to allocate when resizing the vector
*/
#define VECTOR_MINIMUM_EXTRA_CAPACITY 20


/*
* @enum VectorFlags
* @brief Flags for the DynamicVector struct
*/
typedef enum 
{
    VECTOR_FLAG_DECREMENT_PEEK = 0b00000001 // @brief Decrement the peek index after peeking an element
} VectorFlags;

/*
* @struct DynamicVector
* @brief A dynamic vector struct that can store any type of data
* @details The struct contains a data buffer, the current peek index, the current read index, the max index, the total element count, the vector flags, the size of one element in bytes, and a vector of saved states
* @var void* data_buffer: The data buffer of the vector
* @var int peek_index: The next element index to be read by `peek`
* @var int read_index: The current read index
* @var int max_index: The max allocated index before needing resize
* @var int element_count: The total elements stored
* @var int flag_settings: The vector flags
* @var size_t element_size: The size of one element in bytes
* @var struct DynamicVector* saved_states: The vector of saved states (struct only, not data)
*/
typedef struct DynamicVector
{
    void* data_buffer;
    int peek_index;
    int read_index;
    int max_index;
    int element_count;
    int flag_settings;
    size_t element_size;
    struct DynamicVector* saved_states;
} DynamicVector;


/*
* @fn create_vector
* @brief Creates a new DynamicVector struct
* @details Creates a dynamic vector with a specified element size and initializes a secondary vector for saved states.  
* @param element_size The size of one element in bytes
* @return A pointer to the newly created DynamicVector struct
*/
DynamicVector* create_vector(size_t element_size);
/*
* @fn destroy_vector
* @brief Destroys a DynamicVector struct
* @details Safely frees the memory allocated for a dynamic vector, including its data buffer and saved states, before deallocating itself.
* @param vector The DynamicVector struct to destroy
* @return void
*/
void destroy_vector(DynamicVector* vector);
/*
* @fn get_element_at
* @brief Gets an element at a specific index
* @details Returns a pointer to the element at the given index in the dynamic vector or NULL if the index is out of bounds.
* @param vector The DynamicVector struct to get the element from
* @param index The index of the element to get
* @return A pointer to the element at the specified index
*/
void* get_element_at(DynamicVector* vector, int index);
/*
* @fn peek_element_pointer_at
* @brief Peeks an element at a specific index without incrementing the peek index
* @details Returns a pointer to the element stored at the given index, dereferencing it if valid, or NULL if out of bounds.
* @param vector The DynamicVector struct to peek the element from
* @param index The index of the element to peek
* @return A pointer to the element at the specified index
*/
void* peek_element_pointer_at(DynamicVector* vector, int index);
/*
* @fn peek_element_without_increment
* @brief Peeks an element without incrementing the peek index
* @details Returns a pointer to the current peek index element without modifying the index, or NULL if out of bounds.
* @param vector The DynamicVector struct to peek the element from
* @return A pointer to the next element to be read
*/
void* peek_element_without_increment(DynamicVector* vector);
/*
* @fn peek_element
* @brief Peeks an element and increments the peek index
* @details Returns a pointer to the current peek index element and increments the peek index if valid, or NULL if out of bounds.
* @param vector The DynamicVector struct to peek the element from
* @return A pointer to the next element to be read
*/
void* peek_element(DynamicVector* vector);
/*
* @fn peek_element_by_index
* @brief Peeks an element at a specific index
* @details Returns a pointer to the element at the specified index, dereferencing it if valid, or NULL if out of bounds.
* @param vector The DynamicVector struct to peek the element from
* @param index The index of the element to peek
* @return A pointer to the element at the specified index
*/
void* peek_element_by_index(DynamicVector* vector, int index);
/*
* @fn set_vector_flag
* @brief Sets a flag in the DynamicVector struct
* @details Sets the specified flag in the vector's flag settings using bitwise OR to ensure it is enabled.
* @param vector The DynamicVector struct to set the flag in
* @param flag The flag to set
* @return void
*/
void set_vector_flag(DynamicVector* vector, int flag);
/*
* @fn unset_vector_flag
* @brief Unsets a flag in the DynamicVector struct
* @details Unsets the specified flag in the vector's flag settings using bitwise AND with bitwise NOT to disable it.
* @param vector The DynamicVector struct to unset the flag in
* @param flag The flag to unset
* @return void
*/
void unset_vector_flag(DynamicVector* vector, int flag);
/*
* @fn peek_pointer
* @brief Peeks a pointer to the next element to be read
* @details Retrieves the next element's pointer in the vector, increments the peek index, and returns the dereferenced value or NULL if invalid.
* @param vector The DynamicVector struct to peek the pointer from
* @return A pointer to the next element to be read
*/
void* peek_pointer(DynamicVector* vector);
/*
* @fn set_peek_index
* @brief Sets the peek index to a specific index
* @details Sets the peek index of the vector to the specified value without validation.
* @param vector The DynamicVector struct to set the peek index in
* @param index The index to set the peek index to
* @return void
*/
void set_peek_index(DynamicVector* vector, int index);
/*
* @fn set_peek_index_to_end
* @brief Sets the peek index to the end of the vector
* @details Sets the peek index to the last read element by assigning it to read_index - 1.
* @param vector The DynamicVector struct to set the peek index in
* @return void
*/
void set_peek_index_to_end(DynamicVector* vector);
/*
* @fn push_element
* @brief Pushes an element to the end of the vector
* @details Adds a new element to the dynamic vector, expanding memory if needed, and increments the element count.
* @param vector The DynamicVector struct to push the element to
* @param element A pointer to the element to push
* @return void
*/
void push_element(DynamicVector* vector, void* element);
/*
* @fn insert_element_at
* @brief Inserts an element at a specific index
* @details Inserts an element at the specified index in the vector, shifting existing elements forward and expanding capacity if needed.
* @param vector The DynamicVector struct to insert the element to
* @param index The index to insert the element at
* @param element A pointer to the element to insert
* @return void
*/
void insert_element_at(DynamicVector* vector, int index, void* element);
/*
* @fn remove_last_element
* @brief Removes the last element from the vector
* @details Removes the last element from the vector by decrementing element and read index counts, if not empty.
* @param vector The DynamicVector struct to remove the element from
* @return void
*/
void remove_last_element(DynamicVector* vector);
/*
* @fn remove_last_peeked_element
* @brief Removes the last element peeked from the vector
* @details Removes the last peeked element by ensuring a valid peek index and calling remove_element_at().
* @param vector The DynamicVector struct to remove the element from
* @return void
*/
void remove_last_peeked_element(DynamicVector* vector);
/*
* @fn remove_element_at
* @brief Removes an element at a specific index
* @details Removes an element at the specified index by shifting subsequent elements left and updating vector state.  
* @param vector The DynamicVector struct to remove the element from
* @param index The index of the element to remove
* @return void
*/
void remove_element_at(DynamicVector* vector, int index);
/*
* @fn get_last_element
* @brief Gets the last element in the vector
* @details Retrieves the last element in the vector after verifying bounds to ensure safe access.
* @param vector The DynamicVector struct to get the element from
* @return A pointer to the last element in the vector
*/
void* get_last_element(DynamicVector* vector);
/* 
* @fn get_last_element_or_null
* @brief Gets the last element in the vector or NULL if the vector is empty
* @detailsReturns the last element if within bounds; otherwise, returns NULL.
* @param vector The DynamicVector struct to get the element from
* @return A pointer to the last element in the vector or NULL if the vector is empty
*/
void* get_last_element_or_null(DynamicVector* vector);
/*
* @fn get_last_element_pointer
* @brief Gets a pointer to the last element in the vector
* @details Retrieves the last element's pointer if it exists; otherwise, returns NULL.
* @param vector The DynamicVector struct to get the element from
* @return A pointer to the last element in the vector
*/
void* get_last_element_pointer(DynamicVector* vector);
/*
* @fn get_last_element_pointer_or_null
* @brief Gets a pointer to the last element in the vector or NULL if the vector is empty
* @details Returns the pointer to the last element if it exists; otherwise, returns NULL.  
* @param vector The DynamicVector struct to get the element from
* @return A pointer to the last element in the vector or NULL if the vector is empty
*/
void* get_last_element_pointer_or_null(DynamicVector* vector);
/*
* @fn convert_vector_to_string
* @brief Converts the vector to a string
* @details Converts a DynamicVector of integers into a string representation using a static buffer (unsafe for large vectors or concurrent calls).
* @param vector The DynamicVector struct to convert
* @return A string representation of the vector
*/
const char* convert_vector_to_string(DynamicVector* vector);
/*
* @fn is_vector_empty
* @brief Checks if the vector is empty
* @details Checks if the DynamicVector is empty by verifying if element_count is zero.
* @param vector The DynamicVector struct to check
* @return True if the vector is empty, false otherwise
*/
bool is_vector_empty(DynamicVector* vector);
/*
* @fn clear_vector
* @brief Clears the vector
* @details Clears the DynamicVector by removing all elements one by one.
* @param vector The DynamicVector struct to clear
* @return void
*/
void clear_vector(DynamicVector* vector);
/*
* @fn get_element_count
* @brief Gets the total element count in the vector
* @details Returns the number of elements currently stored in the DynamicVector.
* @param vector The DynamicVector struct to get the element count from
* @return The total element count in the vector
*/
int get_element_count(DynamicVector* vector);
/*
* @fn read_vector_from_file
* @brief Reads elements from a file into the vector
* @details Reads element_count elements from a file into the DynamicVector and returns the number of elements successfully read.
* @param vector The DynamicVector struct to read elements into
* @param element_count The number of elements to read
* @param file_pointer The file pointer to read from
* @return The number of elements read
*/
int read_vector_from_file(DynamicVector* vector, int element_count, FILE* file_pointer);
/*
* @fn get_vector_data_pointer
* @brief Gets a pointer to the data buffer of the vector
* @details Returns a pointer to the underlying data buffer of the DynamicVector.
* @param vector The DynamicVector struct to get the data buffer from
* @return A pointer to the data buffer of the vector
*/
void* get_vector_data_pointer(DynamicVector* vector);
/*
* @fn insert_vector_at
* @brief Inserts a vector into another vector at a specific index
* @details Inserts all elements from source_vector into destination_vector at the specified index.
* @param destination_vector The destination vector to insert into
* @param source_vector The source vector to insert
* @param destination_index The index to insert the source vector at
* @return 0 if successful, -1 otherwise
*/
int insert_vector_at(DynamicVector* destination_vector, DynamicVector* source_vector, int destination_index);
/*
* @fn remove_element_by_data_address
* @brief Removes an element by its data address
* @details Removes the element at the specified memory address from the vector and returns its index, or -1 if not found.
* @param vector The DynamicVector struct to remove the element from
* @param element_address The address of the element to remove
* @return The index of the removed element, -1 if not found
*/
int remove_element_by_data_address(DynamicVector* vector, void* element_address);
/*
* @fn remove_element_by_value
* @brief Removes an element by its value
* @details Removes the first occurrence of the specified value from the vector and returns its index, or -1 if not found.
* @param vector The DynamicVector struct to remove the element from
* @param value The value of the element to remove
* @return The index of the removed element, -1 if not found
*/
int remove_element_by_value(DynamicVector* vector, void* value);
/*
* @fn remove_element_by_index
* @brief Removes an element by its index
* @details Removes an element at the specified index by shifting subsequent elements left and updating vector metadata.
* @param vector The DynamicVector struct to remove the element from
* @param index The index of the element to remove
* @return void
*/
void remove_element_by_index(DynamicVector* vector, int index);
/*
* @fn move_peek_pointer_backward
* @brief Moves the peek pointer backward
* @details Moves the peek pointer one step backward if it's not already at the beginning.
* @param vector The DynamicVector struct to move the peek pointer in
* @return void
*/
void move_peek_pointer_backward(DynamicVector* vector);
/*
* @fn get_next_insert_index
* @brief Gets the next insert index in the vector
* @details Returns the next available index for inserting a new element in the vector.
* @param vector The DynamicVector struct to get the next insert index from
* @return The next insert index in the vector
*/
int get_next_insert_index(DynamicVector* vector);
/*
* @fn save_vector_state
* @brief Saves the current state of the vector
* @details Saves the current state of the vector by creating a copy of its structure.
* @param vector The DynamicVector struct to save the state of
* @return void
*/
void save_vector_state(DynamicVector* vector);
/*
* @fn restore_vector_state
* @brief Restores the last saved state of the vector
* @details Restores the vector to its previously saved state and frees the saved state memory.
* @param vector The DynamicVector struct to restore the state of
* @return void
*/
void restore_vector_state(DynamicVector* vector);
/*
* @fn discard_last_saved_state
* @brief Discards the last saved state of the vector
* @details Discards the last saved state of the vector and frees its memory.
* @param vector The DynamicVector struct to discard the state of
* @return void
*/
void discard_last_saved_state(DynamicVector* vector);
/*
* @fn get_vector_element_size
* @brief Gets the size of one element in the vector
* @details Discards the last saved state of the vector and frees its memory.
* @param vector The DynamicVector struct to get the element size from
* @return The size of one element in the vector
*/
size_t get_vector_element_size(DynamicVector* vector);
/*
* @fn clone_vector
* @brief Clones a vector
* @details Creates and returns a deep copy of the given vector.
* @param vector The DynamicVector struct to clone
* @return A pointer to the cloned DynamicVector struct
*/
DynamicVector* clone_vector(DynamicVector* vector);

#endif
