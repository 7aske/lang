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
			return AST_EQUALITY;
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
		case TOK_LBRACE:
			return AST_BLOCK;
		default:
			return AST_NODE;
	}
}

Ast_Node* ast_node_new(Token* token) {
	Ast_Node* ast_node = (Ast_Node*) calloc(1, sizeof(Ast_Node));
	ast_node->type = convert_token_to_ast_node_type(token->token);
	// We do this only in the case of block nodes
	if (ast_node->type == AST_BLOCK) {
		ast_node->precedence = 0;
		ast_node->size = 0;
		ast_node->capacity = 16;
		ast_node->nodes = (Ast_Node**) calloc(1, sizeof(Ast_Node*));
	}
	return ast_node;
}

// Function for adding parsed AST nodes to the block node
inline Ast_Node* ast_block_node_add_node(Ast_Node* block_node, Ast_Node* ast_node){
	assert(block_node->type == AST_BLOCK);
	memcpy(block_node->nodes + block_node->size++, &ast_node, sizeof(Ast_Node**));
	if (block_node->size == block_node->capacity) {
		block_node->capacity += 2;
		block_node->nodes = reallocarray(block_node->nodes, block_node->capacity, sizeof(Ast_Node**));
	}
	return *(block_node->nodes + block_node->size - 1);

}
