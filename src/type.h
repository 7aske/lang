//
// Created by nik on 2/8/22.
//

#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include <stdlib.h>

typedef enum primitive {
	TOK_P_NONE = 0,
	TOK_P_VOID,
	TOK_P_CHAR, // 1-byte
	TOK_P_SHORT,// 2-byte
	TOK_P_INT,  // 4-byte
	TOK_P_LONG, // 8-byte
	__TOK_P_SIZE
} Primitive;

static const int primitive_sizes[__TOK_P_SIZE][2] = {
	{TOK_P_NONE,  0},
	{TOK_P_VOID,  0},
	{TOK_P_CHAR,  1},
	{TOK_P_SHORT, 2},
	{TOK_P_INT,   4},
	{TOK_P_LONG,  8},
};

typedef struct primitive_type {
	const char* iden;
	Primitive type;
} Primitive_Type;

static const Primitive_Type primitive_types[] = {
	{NULL,   TOK_P_NONE},
	{"void", TOK_P_VOID},
	{"bool", TOK_P_CHAR},
	{"s8",   TOK_P_CHAR},
	{"s16",  TOK_P_SHORT},
	{"s32",  TOK_P_INT},
	{"int",  TOK_P_INT},
	{"s64",  TOK_P_LONG},
	{"u8",   TOK_P_CHAR},
	{"u16",  TOK_P_SHORT},
	{"u32",  TOK_P_INT},
	{"u64",  TOK_P_LONG},
};


typedef struct type {

} Type;

#define PRIMITIVE_TYPES_LEN (sizeof(primitive_types)/sizeof(Primitive_Type))

#endif //LANG_TYPE_H
