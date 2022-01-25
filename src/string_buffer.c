//
// Created by nik on 1/25/22.
//
#include "string_buffer.h"
#include <string.h>

// Creates a new heap allocated instance of a string buffer.
String_Buffer* string_buffer_new() {
	String_Buffer* string_buffer =
		(String_Buffer*) calloc(1, sizeof(String_Buffer));
	string_buffer->size = 0;
	string_buffer->capacity = STRING_BUFFER_CAPACITY;
	string_buffer->data = (char*)
		calloc(string_buffer->capacity, STRING_BUFFER_SIZEOF);

	return string_buffer;
}

// Extends the capacity of String_Buffer and reallocates memory
inline void string_buffer_expand(String_Buffer* string_buffer) {
	if (string_buffer->size == string_buffer->capacity) {
		string_buffer->capacity *= 2;
		string_buffer->data = realloc(string_buffer->data,
										   string_buffer->capacity * STRING_BUFFER_SIZEOF);
	}
}

inline void string_buffer_append_char(String_Buffer* string_buffer, char c) {
	string_buffer_expand(string_buffer);
	*(string_buffer->data + string_buffer->size++) = c;
	*(string_buffer->data + string_buffer->size) = '\0';
}

inline void string_buffer_free(String_Buffer* string_buffer) {
	if (string_buffer->data){
		free(string_buffer->data);
	}
	free(string_buffer);
}

// Rather than mem-setting the buffer we do the more efficient route of
// resetting the size and setting the null byte as the first character.
inline void string_buffer_cut(String_Buffer* string_buffer) {
	string_buffer->size = 0;
	*(string_buffer->data) = '\0';
}

inline void string_buffer_clear(String_Buffer* string_buffer) {
	memset(string_buffer->data, 0, string_buffer->size * STRING_BUFFER_SIZEOF);
	string_buffer->size = 0;
}

inline int not_terminated(const char* ptr) {
	 return (ptr != NULL && *ptr != '\0');
}
