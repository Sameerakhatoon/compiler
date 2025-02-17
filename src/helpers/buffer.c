#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

BufferType* create_buffer();

char read_character_from_buffer(BufferType* buffer);

char peek_character_in_buffer(BufferType* buffer);

void expand_buffer(BufferType* buffer, size_t additional_size);

void append_formatted_text(BufferType* buffer, const char* format, ...);

void append_formatted_text_without_null_terminator(BufferType* buffer, const char* format, ...);

void append_character_to_buffer(BufferType* buffer, char character);

void buffer_need_space(BufferType* buffer, size_t size);

void* get_buffer_memory_pointer(BufferType* buffer);

void free_buffer(BufferType* buffer);



BufferType* create_buffer(){
    BufferType* buffer = calloc(sizeof(BufferType), 1);
    buffer->allocated_memory = calloc(BUFFER_REALLOCATION_INCREMENT, 1);
    buffer->current_length = 0;
    buffer->allocated_size = BUFFER_REALLOCATION_INCREMENT;
    return buffer;
}

char read_character_from_buffer(BufferType* buffer){
    if(buffer->read_index >= buffer->current_length){
        return -1;
    }
    char character = buffer->allocated_memory[buffer->read_index];
    buffer->read_index++;
    return character;
}

char peek_character_in_buffer(BufferType* buffer){
    if(buffer->read_index >= buffer->current_length){
        return -1;
    }
    return buffer->allocated_memory[buffer->read_index];
}

void expand_buffer(BufferType* buffer, size_t additional_size){
    buffer->allocated_memory = realloc(buffer->allocated_memory, buffer->allocated_size + additional_size);
    buffer->allocated_size += additional_size;
}

void append_formatted_text(BufferType* buffer, const char* format, ...){
    va_list args;
    va_start(args, format);
    int index = buffer->current_length;
    int length = 2048;
    expand_buffer(buffer, length);
    int actual_length = vsnprintf(&buffer->allocated_memory[index], length, format, args);
    buffer->current_length += actual_length;
    va_end(args);
}

void append_formatted_text_without_null_terminator(BufferType* buffer, const char* format, ...){
    va_list args;
    va_start(args, format);
    int index = buffer->current_length;
    int length = 2048;
    expand_buffer(buffer, length);
    int actual_length = vsnprintf(&buffer->allocated_memory[index], length, format, args);
    buffer->current_length += actual_length - 1;
    va_end(args);
}

void append_character_to_buffer(BufferType* buffer, char character){
    buffer_need_space(buffer, sizeof(char));
    buffer->allocated_memory[buffer->current_length] = character;
    buffer->current_length++;
}

void buffer_need_space(BufferType* buffer, size_t size){
    if(buffer->allocated_size<=buffer->current_length+size){
        expand_buffer(buffer, size + BUFFER_REALLOCATION_INCREMENT);
    }
}

void* get_buffer_memory_pointer(BufferType* buffer){
    return buffer->allocated_memory;
}

void free_buffer(BufferType* buffer){
    free(buffer->allocated_memory);
    free(buffer);
}