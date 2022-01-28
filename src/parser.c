#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 1/26/22.
//

#include "parser.h"

#define NEXT_TOKEN(ptrptr) (*ptrptr)++

#define IS_PEEK_OF_TYPE(__ptrptr, __token) (((*__ptrptr) + 1)->token == __token)
#define IS_CURR_OF_TYPE(__ptrptr, __token) ((*__ptrptr)->token == __token)
#define IS_OF_TYPE(__ptrptr, __token) ((*__ptrptr)->token == __token)

Ast_Node* parse_eq_node(Ast_Node* left, Token** token) {
	assert((*token)->token == TOK_EQ || (*token)->token == TOK_NE);
	Ast_Node* equality_node = ast_node_new(NEXT_TOKEN(token));
	equality_node->left = left;
	equality_node->right = parse_expression(token);
	return equality_node;
}

Parser_Result parser_parse(Lexer_Result* lexer_result) {
	// @Temporary
	u32 capacity = 16;
	Parser_Result result = {.size= 0, .nodes = NULL};
	result.nodes = (Ast_Node**) calloc(capacity, sizeof(Ast_Node*));

	Token* parsed_tokens = lexer_result->data;
	Token* end = lexer_result->data + lexer_result->size;

	while (parsed_tokens < end) {
		Ast_Node* node = parse_statement(&parsed_tokens);

		if (node != NULL) {
			// @Temporary
			memcpy(result.nodes + result.size++, &node, sizeof(Ast_Node**));
			if (result.size == capacity) {
				capacity *= 2;
				result.nodes = reallocarray(result.nodes, capacity,
											sizeof(Ast_Node**));
			}
		}

		parsed_tokens++;
	}

	return result;
}

Ast_Node* parse_statement(Token** lexer_result) {
	Ast_Node* ast_node = NULL;

	if ((*lexer_result)->token == TOK_IF) {
		ast_node = parse_if_statement(lexer_result);
	} else {
		ast_node = parse_expression(lexer_result);
	}

	return ast_node;
}

Ast_Node* parse_if_statement(Token** token) {
	assert((*token)->token == TOK_IF);
	Ast_Node* if_statement = ast_node_new(NEXT_TOKEN(token));
	if_statement->middle = parse_expression(token);

	if_statement->left = parse_expression(token);
	if (IS_CURR_OF_TYPE(token, TOK_ELSE)) {
		NEXT_TOKEN(token); // skip TOK_ELSE
		if_statement->right = parse_expression(token);
	}

	return if_statement;
}

Ast_Node* parse_block_node(Token** token) {
	assert((*token)->token == TOK_LBRACE);
	Ast_Node* block_node = ast_node_new(NEXT_TOKEN(token));
	while(!IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		Ast_Node* ast_node = parse_statement(token);
		if (ast_node == NULL) continue;
		ast_block_node_add_node(block_node, ast_node);
	}
	NEXT_TOKEN(token); // skip TOK_RBRACE

	return block_node;
}

Ast_Node* parse_expression(Token** token) {
	Ast_Node* node = NULL;

	if (IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		node = parse_block_node(token);
	} else if (IS_PEEK_OF_TYPE(token, TOK_AND) ||
			   IS_PEEK_OF_TYPE(token, TOK_OR)) {
		assert(false/*Not implemented*/);
	} else if (IS_PEEK_OF_TYPE(token, TOK_EQ) ||
			   IS_PEEK_OF_TYPE(token, TOK_NE)) {
		node = ast_node_new(NEXT_TOKEN(token));
		node = parse_eq_node(node, token);
	} else {
		node = ast_node_new(NEXT_TOKEN(token));
	}

	return node;
}
#pragma clang diagnostic pop