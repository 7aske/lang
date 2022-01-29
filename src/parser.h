//
// Created by nik on 1/26/22.
//

#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include "lexer.h"
#include "ast.h"

// Struct representing an error report
typedef struct parser_error_report {
	// @Alloc
	char* text;
	Token* source;
} Parser_Error_Report;

typedef struct parser {
	struct {
		u64 size;
		char* text;
	} code;

	struct {
		// @Alloc
		Parser_Error_Report* reports;
		u32 size;
		u32 capacity;
	} error;

} Parser;

typedef struct parser_result {
	u32 size;
	Ast_Node** nodes;
	char* errors;
	u32 error_count;
} Parser_Result;

#define parser_error_foreach(__parser, __code) { \
for(int _i = 0; _i < (__parser)->error.size; ++_i) { \
    Parser_Error_Report it = (__parser)->error.reports[_i]; \
    __code\
}}

#define NEXT_TOKEN(ptrptr) (*(ptrptr))++
#define IS_PEEK_OF_TYPE(__ptrptr, __token) (((*(__ptrptr)) + 1)->token == (__token))
#define IS_CURR_OF_TYPE(__ptrptr, __token) ((*(__ptrptr))->token == (__token))
#define IS_OF_TYPE(__ptrptr, __token) ((*(__ptrptr))->token == (__token))
#define PAD_TO(__end, __str) for (int _i = 0; _i < (__end); _i++) {\
fputs(__str, stderr);\
}

typedef enum ast_result_type {
	AST_NO_ERROR = 0,
	AST_ERROR
} Ast_Error;

typedef struct ast_result {
	Ast_Error error;
	Ast_Node* node;
} Ast_Result;

void parser_new(Parser*, char*);

void parser_print_source_code_location(Parser* parser, Token* token);

void parser_report_error(Parser*, Token*, char*);

Parser_Result parser_parse(Parser*, Lexer_Result*);

Ast_Result parse_statement(Parser*, Token**);

Ast_Result parse_if_statement(Parser*, Token**);

Ast_Result parse_expression(Parser*, Token**);

Ast_Result parse_eq_node(Parser*, Ast_Node*, Token**);

Ast_Result parse_block_node(Parser*, Token**);

void parser_free(Parser* parser);

#endif //LANG_PARSER_H

