//
// Created by nik on 1/26/22.
//
#include "ast.h"

inline Ast_Node_Type convert_token_to_ast_node_type(Token_Type token) {
	switch (token) {
		case TOK_IF:
			return AST_IF;
		case TOK_FOR:
			return AST_FOR;
		case TOK_CONT:
			return AST_CONTINUE;
		case TOK_BREAK:
			return AST_BREAK;
		case TOK_LIT_STR:
		case TOK_LIT_INT:
		case TOK_LIT_FLT:
		case TOK_TRUE:
		case TOK_FALSE:
		case TOK_NULL:
			return AST_LITERAL;
		case TOK_ASSN:
			return AST_ASSIGN;
		case TOK_DQM:
			return AST_DEFAULT;
		case TOK_COL:
			return AST_TYPE_DECL;
		case TOK_ADD:
		case TOK_SUB:
		case TOK_DIV:
		case TOK_MUL:
		case TOK_MOD:
			return AST_ARITHMETIC;
		case TOK_AND:
		case TOK_OR:
		case TOK_NOT:
			return AST_BOOLEAN;
		case TOK_EQ:
		case TOK_NE:
		case TOK_GT:
		case TOK_LT:
		case TOK_GE:
		case TOK_LE:
			return AST_RELATIONAL;
		case TOK_INC:
			return AST_PREINC;
		case TOK_DEC:
			return AST_PREDEC;
		case TOK_IDEN:
			return AST_IDENT;
		default:
			return AST_NODE;
	}
}

Ast_Node* ast_node_new(Token* token) {
	Ast_Node* ast_node = (Ast_Node*) calloc(1, sizeof(Ast_Node));
	ast_node->type = convert_token_to_ast_node_type(token->token);
	ast_node->precedence = 0;
	return ast_node;
}

