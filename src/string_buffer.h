//
// Created by nik on 1/25/22.
//

#ifndef LANG_STRING_BUFFER_H
#define LANG_STRING_BUFFER_H

#include <string.h>
#include <stdlib.h>

#define STRING_BUFFER_CAPACITY (128)
#define STRING_BUFFER_SIZEOF (sizeof(char))

typedef struct string_buffer {
	char* data;
	size_t size;
	size_t capacity;
} String_Buffer;

String_Buffer* string_buffer_new();

void string_buffer_free(String_Buffer* string_buffer);

void string_buffer_expand(String_Buffer* string_buffer);

void string_buffer_append_char(String_Buffer*, char);

void string_buffer_clear(String_Buffer*);

void string_buffer_cut(String_Buffer*);

int not_terminated(const char*);

#endif //LANG_STRING_BUFFER_H
