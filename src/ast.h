//
// Created by nik on 1/26/22.
//

#ifndef LANG_AST_H
#define LANG_AST_H

#include "token.h"
#include "lexer.h"

typedef enum ast_node_type {
	AST_NODE = -1,
	AST_ASSIGN,
	AST_IDENT,
	AST_LITERAL,

	AST_ARITHMETIC,
	AST_RELATIONAL,
	AST_EQUALITY,
	AST_BOOLEAN,
	AST_NOT,
	AST_MINUS,
	AST_PREINC,
	AST_POSTINC,
	AST_PREDEC,
	AST_POSTDEC,

	AST_FOR,
	AST_BREAK,
	AST_CONTINUE,

	AST_IF,
	AST_DEFAULT,
	AST_TERNARY,

	AST_BLOCK,
	AST_TYPE_DECL

} Ast_Node_Type;


typedef struct ast_node {
	union {
		// Regular nodes
		struct {
			struct ast_node* middle;
			struct ast_node* left;
			struct ast_node* right;
		};
		// Block nodes
		struct {
			struct ast_node** nodes;
			u32 size;
			u32 capacity;
		};
	};
	int precedence;
	Ast_Node_Type type;
} Ast_Node;

Ast_Node* ast_node_new(Token* token);
Ast_Node_Type convert_token_to_ast_node_type(Token_Type token);
Ast_Node* ast_block_node_add_node(Ast_Node*, Ast_Node*);

#endif //LANG_AST_H
