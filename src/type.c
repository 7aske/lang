//
// Created by nik on 2/8/22.
//

#include "type.h"

const Type* resolve_primitive_type(const char* iden) {
	// First one is NONE
	for (s32 i = 1; i < PRIMITIVE_TYPES_LEN; ++i) {
		if (strcmp(primitive_types[i].name,iden) == 0) {
			return &primitive_types[i];
		}
	}
	return NULL;
}
