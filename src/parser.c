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

Ast_Result parse_eq_node(Ast_Node* left, Token** token) {
	assert((*token)->token == TOK_EQ || (*token)->token == TOK_NE);
	Ast_Result ast_result= {.type = AST_RESULT_OK, 0};

	Ast_Node* equality_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = equality_node;
	ast_result.node->left = left;

	Ast_Result right_ast_result = parse_expression(token);
	ast_result.node->right = right_ast_result.node;
	ast_result.type = right_ast_result.type;

	return ast_result;
}

Parser_Result parser_parse(Lexer_Result* lexer_result) {
	// @Temporary
	u32 capacity = 16;
	Parser_Result result = {.size= 0, .nodes = NULL};
	result.nodes = (Ast_Node**) calloc(capacity, sizeof(Ast_Node*));

	Token* parsed_tokens = lexer_result->data;
	Token* end = lexer_result->data + lexer_result->size;

	while (parsed_tokens < end) {
		Ast_Result ast_result = parse_statement(&parsed_tokens);

		if (ast_result.type == AST_RESULT_OK) {
			// @Temporary
			memcpy(result.nodes + result.size++, &ast_result.node, sizeof(Ast_Node**));
			if (result.size == capacity) {
				capacity *= 2;
				result.nodes = reallocarray(result.nodes, capacity,
											sizeof(Ast_Node**));
			}
		}

		parsed_tokens++;
	}

	parser_error_foreach({
		fprintf(stderr, "%s\n", it);
	})

	return result;
}

Ast_Result parse_statement(Token** lexer_result) {
	Ast_Result ast_result;

	if ((*lexer_result)->token == TOK_IF) {
		ast_result = parse_if_statement(lexer_result);
	} else {
		ast_result = parse_expression(lexer_result);
	}

	return ast_result;
}

Ast_Result parse_if_statement(Token** token) {
	assert((*token)->token == TOK_IF);
	Ast_Result ast_result= {.type = AST_RESULT_OK, 0};

	Ast_Node* if_statement = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = if_statement;

	Ast_Result middle_ast_result = parse_expression(token);
	ast_result.node->middle = middle_ast_result.node;

	// @Temporary
	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		report_parser_error("Expected token { before IF block")
		ast_result.type = AST_RESULT_ERR;
		return ast_result;
	}

	Ast_Result left_ast_result = parse_expression(token);
	ast_result.node->left = left_ast_result.node;

	if (IS_CURR_OF_TYPE(token, TOK_ELSE)) {
		NEXT_TOKEN(token); // skip TOK_ELSE

		// @Temporary
		if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
			report_parser_error("Expected token { before ELSE block")
			ast_result.type = AST_RESULT_ERR;
			return ast_result;
		}

		Ast_Result right_ast_result = parse_expression(token);
		ast_result.node->right = right_ast_result.node;
	}

	return ast_result;
}

Ast_Result parse_block_node(Token** token) {
	assert((*token)->token == TOK_LBRACE);
	Ast_Result ast_result= {.type = AST_RESULT_OK, 0};
	Ast_Node* block_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = block_node;

	while(!IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		Ast_Result node_result = parse_statement(token);
		if (node_result.type != AST_RESULT_OK) continue;
		ast_block_node_add_node(ast_result.node, node_result.node);
	}
	NEXT_TOKEN(token); // skip TOK_RBRACE

	return ast_result;
}

Ast_Result parse_expression(Token** token) {
	Ast_Result ast_result = {.type = AST_RESULT_OK, 0};

	if (IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		ast_result = parse_block_node(token);
	} else if (IS_PEEK_OF_TYPE(token, TOK_AND) ||
			   IS_PEEK_OF_TYPE(token, TOK_OR)) {
		assert(false/*Not implemented*/);
	} else if (IS_PEEK_OF_TYPE(token, TOK_EQ) ||
			   IS_PEEK_OF_TYPE(token, TOK_NE)) {
		Ast_Node* node = ast_node_new(NEXT_TOKEN(token));
		ast_result = parse_eq_node(node, token);
	} else {
		ast_result.type = AST_RESULT_OK;
		ast_result.node = ast_node_new(NEXT_TOKEN(token));
	}

	return ast_result;
}
#pragma clang diagnostic pop