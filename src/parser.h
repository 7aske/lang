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

Parser_Result parser_parse(Lexer_Result* lexer_result);

Ast_Node* parse_statement(Token** lexer_result);

Ast_Node* parse_if_statement(Token** token);

Ast_Node* parse_expression(Token** token);

Ast_Node* parse_eq_node(Ast_Node* left, Token** token);

Ast_Node* parse_block_node(Token** token);

#endif //LANG_PARSER_H

