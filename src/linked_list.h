//
// Created by nik on 2/9/22.
//

#ifndef LANG_LINKED_LIST_H
#define LANG_LINKED_LIST_H

#include <stdlib.h>
#include <string.h>

#include "stdtypes.h"

/**
 * Struct representing a linked list node.
 *
 * @param next Pointer to the next node in the list.
 * @param data Heap-allocated data node stores.
 */
struct linked_list_node {
	struct linked_list_node* next;
	void* data;
};

#define linked_list_foreach_node(__list, __Type, code) { \
	struct linked_list_node* node = (__list)->start;     \
while (node != NULL) {         \
__Type it = (__Type)node->data;\
code                           \
node = node->next;             \
}}

/**
 * Struct representing a linked list.
 *
 * @param start Start node of the list.
 * @param end   End node of the list.
 * @param size  Size of a single element to be stored.
 * @param count Count of the stored elements.
 */
typedef struct linked_list {
	struct linked_list_node* start;
	struct linked_list_node* end;
	u32 size;
	u32 count;
} Linked_List;

/**
 * Initializes a new linked list.
 *
 * @param linked_list Location where to initialize the list.
 * @param size        Size of a single element to be stored in the list.
 */
void linked_list_new(Linked_List* linked_list, u32 size);

/**
 * Adds an element to the list.
 *
 * @param linked_list this.
 * @param data        Element to add.
 * @return Pointer to the newly added element.
 */
void* linked_list_add(Linked_List* linked_list, void* data);

/**
 * Retrieves an element at index.
 *
 * @param linked_list this.
 * @param index  Index of the element to be retrieved.
 * @return Pointer to the data of the retrieved element.
 */
void* linked_list_get(Linked_List* linked_list, s32 index);

/**
 * Removes an element from the list at index.
 *
 * @param linked_list this.
 * @param index Index at which to remove an element from the list.
 */
void linked_list_remove(Linked_List* linked_list, s32 index);

#endif //LANG_LINKED_LIST_H
