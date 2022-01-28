//
// Created by nik on 1/26/22.
//

#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct parser_result {
	u32 size;
	Ast_Node** nodes;
} Parser_Result;


#define MAX_PARSER_ERRORS (255)
static u32 PARSER_ERROR_COUNT = 0;
// Array of maximum parser errors
static const char* PARSER_ERRORS[MAX_PARSER_ERRORS];
// @Incomplete get source code snippet and source code location
#define report_parser_error(str) if (PARSER_ERROR_COUNT < MAX_PARSER_ERRORS) {PARSER_ERRORS[PARSER_ERROR_COUNT++] = (str);}

#define parser_error_foreach(code) { \
for(int _i = 0; _i < PARSER_ERROR_COUNT; ++_i) { \
    const char* it = PARSER_ERRORS[_i];     \
    code\
}}

typedef enum ast_result_type {
	AST_RESULT_OK = 0,
	AST_RESULT_ERR
} Ast_Result_Type;

typedef struct ast_result {
	Ast_Result_Type type;
	Ast_Node* node;
} Ast_Result;

Parser_Result parser_parse(Lexer_Result* lexer_result);

Ast_Result parse_statement(Token** lexer_result);

Ast_Result parse_if_statement(Token** token);

Ast_Result parse_expression(Token** token);

Ast_Result parse_eq_node(Ast_Node* left, Token** token);

Ast_Result parse_block_node(Token** token);

#endif //LANG_PARSER_H

