//
// Created by nik on 1/25/22.
//
/**
 * String buffer implementation for storing various amounts of textual data.
 */

#ifndef LANG_STRING_BUFFER_H
#define LANG_STRING_BUFFER_H

#include <string.h>
#include <stdlib.h>

#include "stdtypes.h"

#define STRING_BUFFER_CAPACITY (128)
#define STRING_BUFFER_SIZEOF (sizeof(char))

/**
 * Struct representing a String_Buffer.
 * @param data     Actual text data.
 * @param count    Length of the data.
 * @param capacity Maximum length of the data
 * that can be stored before resizing.
 */
typedef struct string_buffer {
	char* data;
	u32 count;
	u32 capacity;
} String_Buffer;

/**
 * Function for dynamically creating a String_Buffer struct.
 *
 * @return Heap-allocated String_Buffer struct.
 */
String_Buffer* string_buffer_new();

/**
 * Frees String_Buffer related data.
 *
 * @param string_buffer this
 */
void string_buffer_free(String_Buffer* string_buffer);

/**
 * Expands the capacity of the buffer if the count is equal to the current
 * capacity.
 *
 * @param string_buffer this
 */
void string_buffer_expand(String_Buffer* string_buffer);

/**
 * Adds a character to the buffer.
 *
 * @param string_buffer this
 * @param character     Character to be added to the buffer.
 */
void string_buffer_append_char(String_Buffer* string_buffer, char character);

/**
 * Clears the associated data for the buffer. Without free-ing.
 *
 * @param string_buffer this.
 */
void string_buffer_clear(String_Buffer* string_buffer);

/**
 * Sets the size to zero without actually mem-setting the data to zero. This
 * is more performant compared to actually clearing the whole buffer/
 *
 * @param string_buffer this
 */
void string_buffer_cut(String_Buffer* string_buffer);

/**
 * Checks whether the pointer is within the bounds of a readable string.
 *
 * @param chr Pointer to the string we want to check.
 *
 * @return True if the pointer is within a readable string.
 */
int not_terminated(const char* chr);

#endif //LANG_STRING_BUFFER_H
