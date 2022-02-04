#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 1/26/22.
//

#include "parser.h"

Ast_Result parse_argument_list(Parser* parser, Token** token);

Ast_Result parse_type_decl_node(Parser* parser, Token** token);

Ast_Result parse_assignment_node(Parser* parser, Token** token);

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

Ast_Result parse_boolean_node(Parser* parser, Token** token) {
	assert((*token)->type == TOK_AND || (*token)->type == TOK_OR);
	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		left_ast_result = parser_create_node(parser, token);
	} else {
		PARSER_POP(&left_ast_result);
	}

	Ast_Result boolean_node = parser_create_node(parser, token);
	boolean_node.node->left = left_ast_result.node;

	Ast_Result right_ast_result = parse_expression(parser, token);
	boolean_node.node->right = right_ast_result.node;
	boolean_node.error = right_ast_result.error;

	if (boolean_node.error == AST_NO_ERROR)
		boolean_node.node = fix_precedence(boolean_node.node);

	return boolean_node;
}

Ast_Result parse_binary_operation_node(Parser* parser, Token** token) {
	assert(IS_CURR_OF_TYPE(token, TOK_GT) ||
		   IS_CURR_OF_TYPE(token, TOK_LT) ||
		   IS_CURR_OF_TYPE(token, TOK_GE) ||
		   IS_CURR_OF_TYPE(token, TOK_LE) ||
		   IS_CURR_OF_TYPE(token, TOK_NE) ||
		   IS_CURR_OF_TYPE(token, TOK_EQ) ||
		   IS_CURR_OF_TYPE(token, TOK_ADD) ||
		   IS_CURR_OF_TYPE(token, TOK_SUB) ||
		   IS_CURR_OF_TYPE(token, TOK_MUL) ||
		   IS_CURR_OF_TYPE(token, TOK_DIV) ||
		   IS_CURR_OF_TYPE(token, TOK_MOD)
	);

	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		left_ast_result = parser_create_node(parser, token);
	} else {
		PARSER_POP(&left_ast_result);
	}

	Ast_Result binary_node = parser_create_node(parser, token);
	binary_node.node->left = left_ast_result.node;

	Ast_Result right_ast_result = parse_expression(parser, token);
	binary_node.node->right = right_ast_result.node;
	binary_node.error = right_ast_result.error;

	if (binary_node.error == AST_NO_ERROR)
		binary_node.node = fix_precedence(binary_node.node);

	return binary_node;
}

Parser_Result parser_parse(Parser* parser, Lexer* lexer) {
	Parser_Result result;
	list_new(&result.nodes, sizeof(Ast_Node*));
	list_new(&result.errors, sizeof(Parser_Error_Report));

	Token* parsed_tokens = lexer->tokens.data;
	Token* end = ((Token*) lexer->tokens.data) + lexer->tokens.count;

	while (parsed_tokens <= end) {
		Ast_Result ast_result = parse_statement(parser, &parsed_tokens);

		if (ast_result.error != AST_NO_ERROR ||ast_result.node == NULL)
			break;

		list_push(&result.nodes, &ast_result.node);
	}

	list_foreach(&parser->errors, Parser_Error_Report*, {
		parser_print_source_code_location(parser, it->source);
		fprintf(stderr, "%s @ %s:%lu:%lu\n",
				it->text,
				"__FILE__",
				it->source->r0,
				it->source->c0);
	})

	// As we are done parsing we can copy error related data to the result struct.
	memcpy(&result.errors, &parser->errors, sizeof(List));

	return result;
}

Ast_Result parse_statement(Parser* parser, Token** lexer_result) {
	Ast_Result ast_result;

	if ((*lexer_result)->type == TOK_INVALID) {
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	if ((*lexer_result)->type == TOK_IF) {
		ast_result = parse_if_statement(parser, lexer_result);
	} else {
		ast_result = parse_expression(parser, lexer_result);
	}

	return ast_result;
}

Ast_Result parse_type_decl_node(Parser* parser, Token** token) {
	Ast_Result type_decl_result;
	Ast_Result iden_result = parser_create_node(parser, token);
	if (iden_result.node->type != AST_IDENT) {
		parser_report_error(parser, *token, "Token should be of type IDENT");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	if (!IS_CURR_OF_TYPE(token, TOK_COL)) {
		parser_report_error(parser, *token, "Expected token :");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	type_decl_result = parser_create_node(parser, token);

	Ast_Result type_result = parser_create_node(parser, token);
	if (type_result.node->type != AST_IDENT) {
		parser_report_error(parser, *token, "Token should be of type IDENT");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	type_decl_result.node->left = iden_result.node;
	type_decl_result.node->right = type_result.node;

	PARSER_PUSH(&type_decl_result);

	return type_decl_result;
}

Ast_Result parse_if_statement(Parser* parser, Token** token) {
	assert((*token)->type == TOK_IF);
	Ast_Result ast_result = parser_create_node(parser, token);

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
	Ast_Result ast_result = parser_create_node(parser, token);

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

	// @Temporary if the previous token is an in identifier that means that
	// we're parsing a function call
	if (IS_CURR_OF_TYPE(token, TOK_LPAREN) &&
		!IS_PREV_OF_TYPE(token, TOK_IDEN)) {
		NEXT_TOKEN(token);
		ast_result = parse_expression(parser, token);

		if (!IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
			parser_report_error(parser, *token, "Expected token )");
			ast_result.error = AST_ERROR;
			return ast_result;
		}

		PARSER_PUSH(&ast_result);

		Ast_Result new = parse_expression(parser, token);

		if (new.node == NULL)
			return ast_result;

		new.node->precedence = S32_MAX;
		new.node = fix_precedence(new.node);
		PARSER_PUSH(&new);
		return new;
	} else if (IS_CURR_OF_TYPE(token, TOK_IDEN)
			   && IS_PEEK_OF_TYPE(token, TOK_COL)
			   && IS_AT_OF_TYPE(token, 2, TOK_IDEN)) {
		ast_result = parse_type_decl_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		return parse_block_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		parser_report_error(parser, *token, "Unexpected token }");
		ast_result.error = AST_ERROR;
		return ast_result;
	} else if (IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		NEXT_TOKEN(token);
	} else if (!IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		ast_result = parser_create_node(parser, token);
	}

	if (IS_CURR_OF_TYPE(token, TOK_LPAREN)) {
		ast_result = parse_argument_list(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_ASSN)) {
		ast_result = parse_assignment_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_AND) ||
			   IS_CURR_OF_TYPE(token, TOK_OR)) {
		ast_result = parse_boolean_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_EQ) ||
			   IS_CURR_OF_TYPE(token, TOK_NE) ||
			   IS_CURR_OF_TYPE(token, TOK_GT) ||
			   IS_CURR_OF_TYPE(token, TOK_LT) ||
			   IS_CURR_OF_TYPE(token, TOK_GE) ||
			   IS_CURR_OF_TYPE(token, TOK_LE) ||
			   IS_CURR_OF_TYPE(token, TOK_ADD) ||
			   IS_CURR_OF_TYPE(token, TOK_SUB) ||
			   IS_CURR_OF_TYPE(token, TOK_MUL) ||
			   IS_CURR_OF_TYPE(token, TOK_DIV) ||
			   IS_CURR_OF_TYPE(token, TOK_MOD)) {
		// All of these are for now parsed as a generic binary operation.
		ast_result = parse_binary_operation_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_SCOL)) {
		// We skip the expression terminating token and return the so far parsed
		// expression.
		NEXT_TOKEN(token);
		return ast_result;
	}

	return ast_result;
}

Ast_Result parse_assignment_node(Parser* parser, Token** token) {
	assert((*token)->type == TOK_ASSN);

	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		left_ast_result = parser_create_node(parser, token);
	} else {
		PARSER_POP(&left_ast_result);
	}
	Ast_Result result = parser_create_node(parser, token);
	Ast_Result right_ast_result = parse_expression(parser, token);
	if (right_ast_result.error != AST_NO_ERROR) {
		result.error = right_ast_result.error;
		return result;
	}
	result.node->left = left_ast_result.node;
	result.node->right = right_ast_result.node;

	return result;
}

// @ToDo implement
Ast_Result parse_argument_list(Parser* parser, Token** token) {
	Ast_Result result;
	while (!IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		NEXT_TOKEN(token);
	}
	return result;
}

inline void parser_report_error(Parser* parser, Token* token, char* error_text) {
	Parser_Error_Report error_report;
	error_report.text = error_text;
	error_report.source = token;
	list_push(&parser->errors, &error_report);
}

inline void parser_new(Parser* parser, char* code) {
	parser->code.size = strlen(code);
	parser->code.text = code;
	list_new(&parser->errors, sizeof(Parser_Error_Report));
	stack_new(&parser->node_stack, sizeof(Ast_Result));
}

inline void parser_free(Parser* parser) {
	// @Incomplete free report.text, node_stack
	list_free(&parser->errors);
}

inline Ast_Node* fix_precedence(Ast_Node* node) {
	// Tree representation of the expression
	// 5 * 6 + 7
	//
	//     *
	//    / \
	//   5   +
	//      / \
	//     6   7
	// Initially parsed expression will be evaluated starting with the +
	// operator which is incorrect. Tree must be rewritten into the
	// following form:
	//      +
	//     / \
	//    *   7
	//   / \
	//  5   6
	// This way the first evaluation will occur at the * node which is required
	// to correctly evaluate the + node.

	// First step is to set + node as the root one:
	//      +
	//     / \
	//    6   7
	// Second step is to put original node as the left child of the root node:
	//      +
	//     / \
	//    *   7
	//   / \
	//  5   + <- we have infinite recursion here
	// Last step is to set left child of the original node as the previous
	// right child of the new root node:
	//      +
	//     / \
	//    *   7
	//   / \
	//  5   6
	Ast_Node* retval = NULL;
	if (node->right != NULL
		&& node->precedence > node->right->precedence
		&& node->right->precedence != 0) {
		retval = node->right;
		Ast_Node* tmp = retval->left;
		retval->left = node;
		node->right = tmp;
	} else {
		retval = node;
	}

	return retval;
}

Ast_Result parser_create_node(Parser* parser, Token** token) {
	Ast_Result ast_result = {
		.error= AST_NO_ERROR,
		.node= ast_node_new(NEXT_TOKEN(token))
	};

	stack_push(&parser->node_stack, &ast_result);
	return ast_result;
}

#pragma clang diagnostic pop