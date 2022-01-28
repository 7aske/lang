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

#endif //LANG_PARSER_H
