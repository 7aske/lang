//
// Created by nik on 2/8/22.
//

#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include <stdlib.h>
#include <string.h>

#include "stdtypes.h"

#define TYPE_CHAR_SIZE  1
#define TYPE_BOOL_SIZE  TYPE_CHAR_SIZE
#define TYPE_SHORT_SIZE (2 * TYPE_CHAR_SIZE)
#define TYPE_INT_SIZE   (4 * TYPE_CHAR_SIZE)
#define TYPE_LONG_SIZE  (8 * TYPE_CHAR_SIZE)

#define TYPE_POINTER   0x00000001
#define TYPE_UNSIGNED  0x00000002
#define TYPE_PRIMITIVE 0x00000004
#define TYPE_IMMUTABLE 0x00000008
#define TYPE_DECIMAL   0x00000010
#define TYPE_ARRAY     0x00000020

typedef struct type {
	char* name;
	u32 size;
	u32 flags;
	u32 ptr_size;
	u32 elements;
	struct type* type;
} Type;

// Base type struct. Technically not a primitive.
static Type type_type =
	{
		.name="type",
		.size=TYPE_LONG_SIZE,
		.flags=TYPE_PRIMITIVE|TYPE_IMMUTABLE,
		.ptr_size=TYPE_LONG_SIZE,
		.type=&type_type,
		.elements=0
	};
// @Optimization this calls to be a hash table
// @formatter:off
static const Type primitive_types[] = {
	{.name=NULL,   .size=0,               .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_IMMUTABLE},
	{.name="void", .size=0,               .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_IMMUTABLE},
	{.name="bool", .size=TYPE_BOOL_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="int",  .size=TYPE_INT_SIZE,   .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="s8",   .size=TYPE_CHAR_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="s16",  .size=TYPE_SHORT_SIZE, .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="s32",  .size=TYPE_INT_SIZE,   .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="s64",  .size=TYPE_LONG_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE               },
	{.name="u8",   .size=TYPE_CHAR_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_UNSIGNED },
	{.name="u16",  .size=TYPE_SHORT_SIZE, .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_UNSIGNED },
	{.name="u32",  .size=TYPE_INT_SIZE,   .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_UNSIGNED },
	{.name="u64",  .size=TYPE_LONG_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_UNSIGNED },
	// @ToDo float64
	{.name="float",.size=TYPE_LONG_SIZE,  .type=&type_type, .elements=1, .flags=TYPE_PRIMITIVE|TYPE_UNSIGNED|TYPE_DECIMAL },
};

static const Type builtin_types[] = {
	{.name="string", .size=TYPE_LONG_SIZE, .type=&type_type, .elements=0, .flags=TYPE_IMMUTABLE|TYPE_POINTER},
};
// @formatter:on

#define BUILTIN_TYPES_LEN   (sizeof(builtin_types)/sizeof(Type))
#define PRIMITIVE_TYPES_LEN (sizeof(primitive_types)/sizeof(Type))

const Type* resolve_primitive_type(const char* iden);

#endif //LANG_TYPE_H
