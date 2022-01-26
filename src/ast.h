//
// Created by nik on 1/26/22.
//

#ifndef LANG_AST_H
#define LANG_AST_H

#include "token.h"
#include "lexer.h"

typedef enum ast_node_type {
	AstNode,
	AstUnaryNode,
	AstIdentifierNode,
	AstArithmeticNode,
	AstAssignmentNode,
	AstTypeDeclarationNode,
	AstBlockNode,
	AstBooleanNode,
	AstBreakNode,
	AstContinueNode,
	AstDefaultNode,
	AstEqualityNode,
	AstIfNode,
	AstLiteralNode,
	AstMinusNode,
	AstNotNode,
	AstRelationalNode,
	AstSymbolNode,
	AstForNode,
} Ast_Node_Type;


typedef struct ast_node {
	struct ast_node* middle;
	struct ast_node* left;
	struct ast_node* right;
	int precedence;
	Ast_Node_Type type;
} Ast_Node;

Ast_Node* ast_node_new(Parsed_Token* token);
Ast_Node_Type convert_token_to_ast_node_type(Token token);

#endif //LANG_AST_H
