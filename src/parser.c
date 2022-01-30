#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 1/26/22.
//

#include "parser.h"

inline void parser_print_source_code_location(Parser* parser, Token* token) {
	u32 start_col = token->c0;
	u32 start_row = token->r0;
	u32 end_col = token->c1;

	char* code = parser->code.text;
	char* print_start = NULL;

	while (1) {

		if (*code == '\0')
			start_row--;

		if (*code == '\n')
			start_row--;

		if (start_row == 1)
			break;

		code++;
	}

	print_start = code;

	while (*print_start != '\n') {
		if (*print_start == '\0') {
			fputc('\n', stderr);
			break;
		} else {
			fputc(*print_start, stderr);
			print_start++;
		}
	}

	PAD_TO(start_col - 1, " ")
	PAD_TO(end_col - start_col, "^")
	fputc('\n', stderr);
	PAD_TO(start_col - 1, " ")
	fputc('|', stderr);
	fputc('\n', stderr);
	PAD_TO(start_col - 1, "─")
	fputs("┘", stderr);
	fputc('\n', stderr);
}

Ast_Result parse_boolean_node(Parser* parser, Ast_Node* left, Token** token) {
	assert((*token)->type == TOK_AND || (*token)->type == TOK_OR);
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	Ast_Node* boolean_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = boolean_node;
	ast_result.node->left = left;

	Ast_Result right_ast_result = parse_expression(parser, token);
	ast_result.node->right = right_ast_result.node;
	ast_result.error = right_ast_result.error;

	if (ast_result.error == AST_NO_ERROR)
		ast_result.node = fix_precedence(ast_result.node);

	return ast_result;
}

Ast_Result parse_relational_node(Parser* parser, Ast_Node* left, Token** token) {
	assert((*token)->type == TOK_GT ||
		   (*token)->type == TOK_LT ||
		   (*token)->type == TOK_GE ||
		   (*token)->type == TOK_LE);

	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	Ast_Node* relational_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = relational_node;
	ast_result.node->left = left;

	Ast_Result right_ast_result = parse_expression(parser, token);
	ast_result.node->right = right_ast_result.node;
	ast_result.error = right_ast_result.error;

	if (ast_result.error == AST_NO_ERROR)
		ast_result.node = fix_precedence(ast_result.node);

	return ast_result;
}

Ast_Result parse_eq_node(Parser* parser, Ast_Node* left, Token** token) {
	assert((*token)->type == TOK_EQ || (*token)->type == TOK_NE);
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	Ast_Node* equality_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = equality_node;
	ast_result.node->left = left;

	Ast_Result right_ast_result = parse_expression(parser, token);
	ast_result.node->right = right_ast_result.node;
	ast_result.error = right_ast_result.error;

	if (ast_result.error == AST_NO_ERROR)
		ast_result.node = fix_precedence(ast_result.node);

	return ast_result;
}

Parser_Result parser_parse(Parser* parser, Lexer_Result* lexer_result) {
	// @Temporary
	u32 capacity = 16;
	Parser_Result result = {.size= 0, .nodes = NULL};
	result.nodes = (Ast_Node**) calloc(capacity, sizeof(Ast_Node*));

	Token* parsed_tokens = lexer_result->data;
	Token* end = lexer_result->data + lexer_result->size;

	while (parsed_tokens <= end) {
		Ast_Result ast_result = parse_statement(parser, &parsed_tokens);

		if (ast_result.error != AST_NO_ERROR)
			break;

		// @Temporary
		memcpy(result.nodes + result.size++, &ast_result.node,
			   sizeof(Ast_Node**));
		if (result.size == capacity) {
			capacity *= 2;
			result.nodes = reallocarray(result.nodes, capacity,
										sizeof(Ast_Node**));
		}
	}

	parser_error_foreach(parser, {
		parser_print_source_code_location(parser, it.source);
		fprintf(stderr, "%s @ %s:%lu:%lu\n", it.text, "__FILE__", it.source->r0,
				it.source->c0);
	})

	// @Incomplete return result.errors
	result.error_count = parser->error.size;

	return result;
}

Ast_Result parse_statement(Parser* parser, Token** lexer_result) {
	Ast_Result ast_result;

	if ((*lexer_result)->type == TOK_IF) {
		ast_result = parse_if_statement(parser, lexer_result);
	} else {
		ast_result = parse_expression(parser, lexer_result);
	}

	return ast_result;
}

Ast_Result parse_if_statement(Parser* parser, Token** token) {
	assert((*token)->type == TOK_IF);
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	Ast_Node* if_statement = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = if_statement;

	Ast_Result middle_ast_result = parse_expression(parser, token);
	ast_result.node->middle = middle_ast_result.node;

	// @Temporary
	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		parser_report_error(parser, *token, "Expected token {");
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	Ast_Result left_ast_result = parse_expression(parser, token);
	ast_result.node->left = left_ast_result.node;

	if (IS_CURR_OF_TYPE(token, TOK_ELSE)) {
		NEXT_TOKEN(token); // skip TOK_ELSE

		// @Temporary
		if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
			parser_report_error(parser, *token, "Expected token {");
			ast_result.error = AST_ERROR;
			return ast_result;
		}

		Ast_Result right_ast_result = parse_expression(parser, token);
		ast_result.node->right = right_ast_result.node;
	}

	return ast_result;
}

Ast_Result parse_block_node(Parser* parser, Token** token) {
	assert((*token)->type == TOK_LBRACE);
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};
	Ast_Node* block_node = ast_node_new(NEXT_TOKEN(token));
	ast_result.node = block_node;

	while (!IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		Ast_Result node_result = parse_statement(parser, token);
		if (node_result.error != AST_NO_ERROR) continue;
		ast_block_node_add_node(ast_result.node, node_result.node);
	}
	NEXT_TOKEN(token); // skip TOK_RBRACE

	return ast_result;
}

Ast_Result parse_expression(Parser* parser, Token** token) {
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	if (IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		ast_result = parse_block_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_GT) ||
			   IS_CURR_OF_TYPE(token, TOK_LT) ||
			   IS_CURR_OF_TYPE(token, TOK_GE) ||
			   IS_CURR_OF_TYPE(token, TOK_LE) ||
			   IS_CURR_OF_TYPE(token, TOK_EQ) ||
			   IS_CURR_OF_TYPE(token, TOK_NE)) { // @Temporary
		// None of these tokens should the token in line for parsing
		// in this method. That means that something is wrong. Usually...
		parser_report_error(parser, *token, "Unexpected token");
		ast_result.error = AST_ERROR;
	} else if (IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		parser_report_error(parser, *token, "Unexpected token }");
		ast_result.error = AST_ERROR;
	} else if (IS_PEEK_OF_TYPE(token, TOK_AND) ||
			   IS_PEEK_OF_TYPE(token, TOK_OR)) {
		Ast_Node* node = ast_node_new(NEXT_TOKEN(token));
		ast_result = parse_boolean_node(parser, node, token);
	} else if (IS_PEEK_OF_TYPE(token, TOK_EQ) ||
			   IS_PEEK_OF_TYPE(token, TOK_NE)) {
		Ast_Node* node = ast_node_new(NEXT_TOKEN(token));
		ast_result = parse_eq_node(parser, node, token);
	} else if (IS_PEEK_OF_TYPE(token, TOK_GT) ||
			   IS_PEEK_OF_TYPE(token, TOK_LT) ||
			   IS_PEEK_OF_TYPE(token, TOK_GE) ||
			   IS_PEEK_OF_TYPE(token, TOK_LE)) {
		Ast_Node* node = ast_node_new(NEXT_TOKEN(token));
		ast_result = parse_relational_node(parser, node, token);
	} else {
		ast_result.node = ast_node_new(NEXT_TOKEN(token));
	}

	return ast_result;
}

inline void parser_report_error(Parser* parser, Token* token, char* error_text) {
	Parser_Error_Report error_report;
	error_report.text = error_text;
	error_report.source = token;
	memcpy(parser->error.reports + parser->error.size++,
		   &error_report,
		   sizeof(Parser_Error_Report));
	if (parser->error.size == parser->error.capacity) {
		parser->error.capacity *= 2;
		parser->error.reports =
			reallocarray(parser->error.reports,
						 parser->error.capacity,
						 sizeof(Parser_Error_Report));
	}

}

inline void parser_new(Parser* parser, char* code) {
	parser->code.size = strlen(code);
	parser->code.text = code;
	parser->error.capacity = 16;
	parser->error.size = 0;
	parser->error.reports =
		(Parser_Error_Report*) calloc(parser->error.capacity,
									  sizeof(Parser_Error_Report));
}

inline void parser_free(Parser* parser) {
	// @Incomplete free report.text
	parser->error.size = 0;
	free(parser->error.reports);
}

inline Ast_Node* fix_precedence(Ast_Node* node) {
	Ast_Node* retval = NULL;
	if (node->right->precedence > node->precedence) {
		retval = node->right;
		Ast_Node* right_left = node->right->left;
		node->right->left = node;
		node->right = right_left;
	} else {
		retval = node;
	}

	return retval;
}

#pragma clang diagnostic pop