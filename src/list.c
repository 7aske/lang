//
// Created by nik on 2/1/22.
//

#include "list.h"

inline void list_new(List* dest, u64 size) {
	dest->count = 0;
	dest->__list_size = size;
	dest->capacity = LIST_INITIAL_CAPACITY;
	dest->data = (void*) calloc(dest->capacity, size);
}

inline void* list_push(List* list, void* src) {
	list_realloc(list);
	memcpy(list->data + (list->count++ * list->__list_size), src, list->__list_size);
	return list->data + (list->count - 1) * list->__list_size;
}

inline void list_realloc(List* list) {
	if (list->count == list->capacity) {
		list->capacity *= 2;
		list->data = realloc(list->data, list->capacity * list->__list_size);
	}
}

inline void* list_get(List* list, s64 index) {
	// Cannot be less than zero because it is an unsigned number.
	if (index >= list->count) return NULL;
	return list->data + (index * list->__list_size);
}

inline void list_remove(List* list, s64 index) {
	if (index >= list->__list_size) return;
	void* dest = list->data + (index * list->__list_size);
	void* src = list->data + (index + 1) * list->__list_size;
	u64 count = (list->count - index - 1) * list->__list_size;
	memmove(dest, src , count);
	list->count--;
}

inline bool list_is_empty(List* list) {
	return list->count == 0;
}

void list_free(List* list) {
	if (list->data != NULL) {
		list->count = 0;
		list->capacity = LIST_INITIAL_CAPACITY;
		free(list->data);
		list->data = NULL;
	}
}
