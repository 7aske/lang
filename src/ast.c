//
// Created by nik on 1/26/22.
//
#include "ast.h"

inline Ast_Node_Type convert_token_to_ast_node_type(Token_Type token) {
	switch (token) {
		case TOK_WHILE:
			return AST_WHILE;
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
		case TOK_SUBASSN:
		case TOK_ADDASSN:
		case TOK_DIVASSN:
		case TOK_MULASSN:
			return AST_ASSN_BINOP;
		case TOK_ADD:
		case TOK_SUB:
		case TOK_DIV:
		case TOK_STAR:
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
		case TOK_LBRACK:
			return AST_ARRAY_INDEX;
		case TOK_FN:
			return AST_FUNC_DEF;
		case TOK_THIN_ARRW:
			return AST_FUNC_RET_TYPE;
		case TOK_IN:
			return AST_IN;
		case TOK_DDOT:
			return AST_ITER;
		case TOK_RETURN:
			return AST_FUNC_RETURN;
		default:
			return AST_NODE;
	}
}

Ast_Node* ast_node_new(Token* token) {
	assert(token->type != TOK_RPAREN);
	Ast_Node* ast_node = (Ast_Node*) calloc(1, sizeof(Ast_Node));
	ast_node->node_type = convert_token_to_ast_node_type(token->type);
	ast_node->precedence = 0; // default

	// We do this only in the case of block nodes
	if (ast_node->node_type == AST_BLOCK) {
		list_new(&ast_node->nodes, sizeof(Ast_Node*));
	}

	if (ast_node->node_type == AST_BOOLEAN) {
		if (token->type == TOK_AND) {
			ast_node->precedence = AST_AND_PRECEDENCE;
		} else if (token->type == TOK_OR) {
			ast_node->precedence = AST_OR_PRECEDENCE;
		}
	} else if (ast_node->node_type == AST_EQUALITY) {
		ast_node->precedence = AST_EQUALITY_PRECEDENCE;
	} else if (ast_node->node_type == AST_RELATIONAL) {
		ast_node->precedence = AST_RELATIONAL_PRECEDENCE;
	} else if (ast_node->node_type == AST_ASSIGN) {
		ast_node->precedence = AST_ASSIGN_PRECEDENCE;
	} else if (ast_node->node_type == AST_ARITHMETIC) {
		if (token->type == TOK_ADD ||
			token->type == TOK_SUB) {
			ast_node->precedence = AST_ADD_SUB_PRECEDENCE;
		} else if (token->type == TOK_STAR ||
				   token->type == TOK_DIV ||
				   token->type == TOK_MOD) {
			ast_node->precedence = AST_MUL_DIV_MOD_PRECEDENCE;
		}
	}

	memcpy(&ast_node->token, token, sizeof(Token));

	return ast_node;
}

// Function for adding parsed AST nodes to the block node
inline Ast_Node* ast_block_node_add_node(Ast_Node* block_node, Ast_Node* ast_node) {
	assert(block_node->node_type == AST_BLOCK || block_node->node_type == AST_ARG_LIST);
	return list_push(&block_node->nodes, &ast_node);
}
