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

const Type* resolve_builtin_type(const char* iden) {
	for (s32 i = 0; i < BUILTIN_TYPES_LEN; ++i) {
		if (strcmp(builtin_types[i].name,iden) == 0) {
			return &builtin_types[i];
		}
	}
	return NULL;
}
