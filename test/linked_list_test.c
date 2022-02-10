//
// Created by nik on 2/1/22.
//

#include <assert.h>
#include <stdio.h>
#include "linked_list.h"

int main(void) {
	Linked_List linked_list;
	linked_list_new(&linked_list, sizeof(s64));
	s64 val = 64;
	linked_list_add(&linked_list, &val);
	linked_list_add(&linked_list, &val);
	assert(*((u64*)linked_list_get(&linked_list, 0)) == val);
	linked_list_remove(&linked_list, 0);
	assert(linked_list_get(&linked_list, 0) != NULL);
}