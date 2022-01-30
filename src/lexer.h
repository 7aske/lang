//
// Created by nik on 1/25/22.
//

#ifndef LANG_LEXER_H
#define LANG_LEXER_H

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "stdtypes.h"
#include "string_buffer.h"


typedef enum lexer_error {
	LEXER_FAILED = -1,
} Lexer_Error;

typedef struct lexer_result {
	Token* data;
	u32 size;
	u32 capacity;
} Lexer_Result;

typedef struct lexer_error_report {
	// @Alloc
	char* text;
	Token_Type type;
	// @Incomplete add end col and end row
	u32 col;
	u32 row;
} Lexer_Error_Report;

typedef struct lexer {
	struct {
		u64 size;
		char* text;
	} code;

	struct {
		// @Alloc
		Lexer_Error_Report* reports;
		u32 size;
		u32 capacity;
	} error;
} Lexer;

// @CopyPaste
#define PAD_TO(__end, __str) for (int _i = 0; _i < (__end); _i++) {\
fputs(__str, stderr);\
}

#define PEEK_NEXT(ptr) *((ptr) + 1)

#define lexer_error_foreach(__lexer, code) { \
for(int _i = 0; _i < (__lexer)->error.size; ++_i) { \
    Lexer_Error_Report it = (lexer)->error.reports[_i];     \
    code\
}}

#define LEXER_RESULT_DATA_FREE(result) {\
for(int _i = 0; _i < (result)->size; ++_i) {\
    Token it = (result)->data[_i];\
	if (it.string_value.data != NULL) {\
		free(it.string_value.data);\
	}\
}\
free((result)->data);\
}

void lexer_new(Lexer*, char*);

u32 lexer_lex(Lexer*, Lexer_Result*);

bool lexer_startof_iden(const char*);

bool lexer_startof_number(const char*);

bool lexer_startof_string(const char*);

bool lexer_startof_char(const char* ptr);

bool lexer_is_iden(const char*);

bool lexer_is_number(const char*);

bool lexer_is_float(const char*);

void lexer_token_new(Token*, Token_Type, u32, u32, u32);

Lexer_Error lexer_eat_char(char** , String_Buffer*);

Lexer_Error lexer_eat_string(char**, String_Buffer*);

u32 lexer_eat_iden(char**, String_Buffer*);

Token_Type lexer_eat_token(char**, String_Buffer*);

u32 lexer_eat_number(char**, String_Buffer*);

void lexer_report_error(Lexer*, Token_Type token_type, u32 col, u32 row, char*);

void lexer_free(Lexer*);

void lexer_print_source_code_location(Lexer* lexer, Lexer_Error_Report*);

#endif //LANG_LEXER_H
