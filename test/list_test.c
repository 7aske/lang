//
// Created by nik on 2/1/22.
//

#include <assert.h>
#include <stdio.h>
#include "list.h"

int main(void) {
	List list;
	list_new(&list, sizeof(s64));
	list_push(&list, &(s64){1});
	list_push(&list, &(s64){2});
	list_push(&list, &(s64){3});

	list_foreach(&list, s64*, {
		printf("%lld", *it);
	})

	s64* ptr = list_get(&list, 0);
	assert(*ptr == 1);
	list_remove(&list, 0);
	ptr = list_get(&list, 0);
	assert(*ptr == 2);
	list_remove(&list, 0);
	ptr = list_get(&list, 0);
	assert(*ptr == 3);
	list_remove(&list, 0);
	ptr = list_get(&list, 0);
	assert(ptr == NULL);
	list_free(&list);
}