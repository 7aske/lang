//
// Created by nik on 2/1/22.
//

#ifndef LANG_LIST_H
#define LANG_LIST_H

#include <stdlib.h>
#include <string.h>
#include "stdtypes.h"

// Initial capacity of the list. Used in list "constructor".
#define LIST_INITIAL_CAPACITY 16

#define list_foreach(__list, __Type, code) { \
for(int _i = 0; _i < (__list)->count; ++_i) { \
    __Type it = (__Type)list_get(__list, _i);\
    code\
}}

/**
 * Standard list struct.
 *
 * @param size     Size of the individual element contained in the list.
 *                 Typically called determined by sizeof(expr).
 * @param count    Count of the elements stored in the list.
 * @param capacity Maximum capacity of the list before it needs to resized.
 * @param data     Heap-allocated array for storing elements in the list.
 */
typedef struct list {
	u64 __list_size;
	s64 count;
	u64 capacity;
	void* data;
} List;

/**
 * Populates the struct pointed to by the dest pointer with required data.
 *
 * @param dest Allocated struct to be populated by the required data.
 * @param size Size of the element the list will be storing.
 */
void list_new(List* dest, u64 size);

/**
 * Function to add an element to the end of the list.
 *
 * @param list this.
 * @param src  Element to be added.
 * @return Pointer to the newly added element.
 */
void* list_push(List* list, void* src);

/**
 * Retrieves an element from the list. Has bounds checking.
 *
 * @param list this.
 * @param index Index of the element in the list.
 * @return Pointer to the element from the list. NULL if out of bounds.
 */
void* list_get(List* list, s64 index);

// Utility macro to auto-cast the result to provided __type.
#define list_get_as(__list, __index, __type) ((__type)list_get(__list, __index))

/**
 * Removes the element at index.
 *
 * @param list this.
 * @param index Index of the element to remove.
 */
void list_remove(List* list, s64 index);

/**
 * Checks whether list is empty.
 *
 * @param list this.
 * @return True if the list count is 0;
 */
bool list_is_empty(List* list);

/**
 * Helper method to reallocated the data buffer when it gets filled up.
 *
 * @param list this.
 */
void list_realloc(List* list);

/**
 * Frees resources allocated by the list.
 *
 * @param list this.
 */
void list_free(List* list);

#endif //LANG_LIST_H
