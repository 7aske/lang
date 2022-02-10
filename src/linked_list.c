//
// Created by nik on 2/9/22.
//

#include <assert.h>
#include "linked_list.h"

void linked_list_new(Linked_List* linked_list, u32 size) {
	linked_list->size = size;
	linked_list->start = NULL;
	linked_list->end = NULL;
	linked_list->count = 0;
}

void* linked_list_add(Linked_List* linked_list, void* data) {
	struct linked_list_node* new_node = (struct linked_list_node*)
		calloc(1, sizeof(struct linked_list_node));
	new_node->data = malloc(linked_list->size);
	memcpy(new_node->data, data, linked_list->size);
	if (linked_list->start == NULL) {
		assert(linked_list->end == NULL);
		linked_list->start = new_node;
		linked_list->end = new_node;
	} else {
		linked_list->end->next = new_node;
		linked_list->end = new_node;
	}
	linked_list->count++;
	return new_node->data;
}

void* linked_list_get(Linked_List* linked_list, s32 index) {
	struct linked_list_node* start = linked_list->start;
	while (index-- > 0) {
		start = start->next;
	}
	if (start == NULL) return NULL;
	return start->data;
}


void linked_list_remove(Linked_List* linked_list, s32 index) {
	if (index < 0 || index >= linked_list->count)
		return;

	struct linked_list_node* to_delete = linked_list->start;
	struct linked_list_node* prev = NULL;
	while (index > 0) {
		prev = to_delete;
		to_delete = to_delete->next;
		index--;
	}

	if (to_delete == linked_list->end) {
		linked_list->end = prev;
	}

	if (to_delete == linked_list->start) {
		linked_list->start = to_delete->next;
	}

	if (prev)
		prev->next = to_delete->next;

	free(to_delete->data);
	free(to_delete);
	linked_list->count--;
}
