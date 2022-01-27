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
} Lexer_Result;

#define MAX_LEXER_ERRORS (255)
static int LEXER_ERROR_COUNT = 0;
static const u8* LEXER_ERRORS[MAX_LEXER_ERRORS];
// @Incomplete get source code snippet and source code location
#define report_lexer_error(str) if (LEXER_ERROR_COUNT < MAX_LEXER_ERRORS) {LEXER_ERRORS[LEXER_ERROR_COUNT++] = (str);}

#define lexer_error_foreach(code) { \
for(int _i = 0; _i < LEXER_ERROR_COUNT; ++_i) { \
    const u8* it = LEXER_ERRORS[_i];     \
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

u32 lexer_lex(char*, Lexer_Result*);

bool lexer_startof_iden(const char*);

bool lexer_startof_number(const char*);

bool lexer_startof_string(const char*);

bool lexer_is_iden(const char*);

bool lexer_is_number(const char*);

bool lexer_is_float(const char*);

void lexer_token_new(Token*, Token_Type, u32, u32, u32);

Lexer_Error lexer_eat_string(char**, String_Buffer*);

u32 lexer_eat_iden(char**, String_Buffer*);

Token_Type lexer_eat_token(char**, String_Buffer*);

u32 lexer_eat_number(char**, String_Buffer*);

#endif //LANG_LEXER_H
