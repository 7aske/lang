//
// Created by nik on 2/1/22.
//

#include <assert.h>
#include <stdio.h>
#include "map.h"

int main(void) {
	Map map;
	s64  val;
	s64* retval;
	map_new(&map, sizeof(s64));
	val= 1;
	map_put(&map, "test", &val);
	assert(map.count == 1);
	retval = (s64*) map_get(&map, "test");
	assert(*retval == val);
	map_remove(&map, "test");
	assert(map_get(&map, "test") == NULL);
	assert(map.count == 0);

	char buf[32];
	for (s32 i = 0; i < 100; ++i) {
		sprintf(buf, "test%ld", i);
		map_put(&map, buf, &i);
	}
	fprintf(stderr, "count: %ld\n", map.count);
	assert(map.count == 100);
	for (s32 i = 0; i < 100; ++i) {
		sprintf(buf, "test%ld", i);
		assert(*(s64*)map_get(&map, buf) == i);
	}
	assert(map.count == 100);
}