//
// Created by nik on 1/26/22.
//
#include "ast.h"

inline Ast_Node_Type convert_token_to_ast_node_type(Token_Type token) {
	switch (token) {
		case TOK_IF:
			return AstIfNode;
		case TOK_FOR:
			return AstForNode;
		case TOK_CONT:
			return AstContinueNode;
		case TOK_BREAK:
			return AstBreakNode;
		case TOK_LIT_STR:
		case TOK_LIT_INT:
		case TOK_LIT_FLT:
		case TOK_TRUE:
		case TOK_FALSE:
		case TOK_NULL:
			return AstLiteralNode;
		case TOK_ASSN:
			return AstAssignmentNode;
		case TOK_DQM:
			return AstDefaultNode;
		case TOK_COL:
			return AstTypeDeclarationNode;
		case TOK_ADD:
		case TOK_SUB:
		case TOK_DIV:
		case TOK_MUL:
		case TOK_MOD:
			return AstArithmeticNode;
		case TOK_AND:
		case TOK_OR:
		case TOK_NOT:
			return AstBooleanNode;
		case TOK_EQ:
		case TOK_NE:
		case TOK_GT:
		case TOK_LT:
		case TOK_GE:
		case TOK_LE:
			return AstRelationalNode;
		case TOK_INC:
		case TOK_DEC:
			return AstUnaryNode;
		case TOK_IDEN:
			return AstIdentifierNode;
		default:
			return AstNode;
	}
}

Ast_Node* ast_node_new(Token* token) {
	Ast_Node* ast_node = (Ast_Node*) calloc(1, sizeof(Ast_Node));
	ast_node->type = convert_token_to_ast_node_type(token->token);
	ast_node->precedence = 0;
	return ast_node;
}

