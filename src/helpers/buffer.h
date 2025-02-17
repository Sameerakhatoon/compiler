/*
* @file buffer.h
* @brief Header file for buffer.c
* @details Contains function prototypes for buffer.c
*/

#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stddef.h>

/*
* @def BUFFER_REALLOCATION_INCREMENT
* @brief Amount by which the buffer expands when it needs more space
*/
#define BUFFER_REALLOCATION_INCREMENT 2000

/*
* @struct BufferType
* @brief Structure to store the buffer information
* @details Contains the allocated memory, read index, current length, and allocated size of the buffer
* @var char* allocated_memory
* Pointer to the dynamically allocated memory
* @var int read_index
* Read position in the buffer
* @var int current_length
* Current length of data stored in the buffer
* @var int allocated_size
* Total allocated memory size
*/
typedef struct BufferType
{
    char* allocated_memory;
    int read_index;
    int current_length;
    int allocated_size;   
} BufferType;

// Function prototypes

/*
* @fn create_buffer
* @brief Function to create a buffer
* @details // Allocates and initializes a buffer with a predefined size, setting memory for storage, tracking length, and allocation size.
* @return Pointer to the buffer
*/
BufferType* create_buffer();
/*
* @fn read_character_from_buffer
* @brief Function to read a character from the buffer
* @details Reads a character from the buffer if available, updates the read index, and returns -1 if out of bounds. 
* @param buffer Pointer to the buffer
* @return Character read from the buffer
*/
char read_character_from_buffer(BufferType* buffer);
/*
* @fn peek_character_in_buffer
* @brief Function to peek at the next character in the buffer
* @details Returns the next character in the buffer without advancing the read index, or -1 if out of bounds.
* @param buffer Pointer to the buffer
* @return Next character in the buffer
*/
char peek_character_in_buffer(BufferType* buffer);
/*
* @fn expand_buffer
* @brief Function to expand the buffer
* @details Expands the buffer by reallocating memory with the specified additional size and updating the allocated size.  
* @param buffer Pointer to the buffer
* @param additional_size Amount by which the buffer should be expanded
* @return void
*/
void expand_buffer(BufferType* buffer, size_t additional_size);
/*
* @fn append_formatted_text
* @brief Function to append formatted text to the buffer
* @details Appends formatted text to the buffer by expanding memory, formatting the string, and updating the current length.
* @param buffer Pointer to the buffer
* @param format Format string for the text to be appended
* @return void
*/
void append_formatted_text(BufferType* buffer, const char* format, ...);
/*
* @fn append_formatted_text_without_null_terminator
* @brief Function to append formatted text to the buffer without null terminator
* @details Appends formatted text to the buffer without counting the null terminator, expands memory, and updates length accordingly.
* @param buffer Pointer to the buffer
* @param format Format stirng for the text to be appended
* @return void
*/
void append_formatted_text_without_null_terminator(BufferType* buffer, const char* format, ...);
/*
* @fn append_character_to_buffer
* @brief Function to append a character to the buffer
* @details Appends a single character to the buffer, ensuring enough space before storing it and updating the length.
* @param buffer Pointer to the buffer
* @param character Character to be appended
* @return void
*/
void append_character_to_buffer(BufferType* buffer, char character);
/*
* @fn get_buffer_memory_pointer
* @brief Function to get the memory pointer of the buffer
* @details Returns a pointer to the allocated memory of the buffer for direct access to its contents.
* @param buffer Pointer to the buffer
* @return void
*/
void* get_buffer_memory_pointer(BufferType* buffer);
/*
* @fn free_buffer
* @brief Function to free the buffer
* @details Frees the allocated memory of the buffer and then deallocates the buffer structure itself.
* @param buffer Pointer to the buffer
* @return void
*/
void free_buffer(BufferType* buffer);

#endif // BUFFER_H
