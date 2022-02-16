#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 1/26/22.
//

#include "parser.h"

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

Ast_Result parse_assn_binop_node(Parser* parser, Token** token) {
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

	PARSER_PUSH(&binary_node);

	return binary_node;
}

Ast_Result parse_binary_operation_node(Parser* parser, Token** token) {
	// @formatter:off
	assert(IS_CURR_OF_TYPE(token, TOK_GT)   ||
		   IS_CURR_OF_TYPE(token, TOK_LT)   ||
		   IS_CURR_OF_TYPE(token, TOK_GE)   ||
		   IS_CURR_OF_TYPE(token, TOK_LE)   ||
		   IS_CURR_OF_TYPE(token, TOK_NE)   ||
		   IS_CURR_OF_TYPE(token, TOK_EQ)   ||
		   IS_CURR_OF_TYPE(token, TOK_ADD)  ||
		   IS_CURR_OF_TYPE(token, TOK_SUB)  ||
		   IS_CURR_OF_TYPE(token, TOK_STAR) ||
		   IS_CURR_OF_TYPE(token, TOK_DIV)  ||
		   IS_CURR_OF_TYPE(token, TOK_MOD)
	);
	// @formatter:on

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

	// Initial scope
	parser_push_scope(parser);

	while (parsed_tokens <= end && parsed_tokens->type != TOK_INVALID) {
		Ast_Result ast_result = parse_statement(parser, &parsed_tokens);

		if (ast_result.error != AST_NO_ERROR || ast_result.node == NULL)
			break;

		list_push(&result.nodes, &ast_result.node);
	}

	list_foreach(&parser->errors, Parser_Error_Report*, {
		print_token_source_code_location(parser->code.text, it->source);
		fprintf(stderr, "%s %s:%lu:%lu\n",
				it->text,
				parser->code.filename,
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
		REPORT_ERROR(*lexer_result, "Invalid token.");
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	if (IS_CURR_OF_TYPE(lexer_result, TOK_IF)) {
		ast_result = parse_if_statement(parser, lexer_result);
	} else if (IS_CURR_OF_TYPE(lexer_result, TOK_WHILE)) {
		ast_result = parse_while_statement(parser, lexer_result);
	} else if (IS_CURR_OF_TYPE(lexer_result, TOK_FN)) {
		ast_result = parse_fn_statement(parser, lexer_result);
	} else if (IS_CURR_OF_TYPE(lexer_result, TOK_FOR)) {
		ast_result = parse_for_statement(parser, lexer_result);
	} else if (IS_CURR_OF_TYPE(lexer_result, TOK_RETURN)) {
		ast_result = parse_return_statement(parser, lexer_result);
	} else {
		while (!IS_CURR_OF_TYPE(lexer_result, TOK_SCOL) &&
			   !IS_CURR_OF_TYPE(lexer_result, TOK_INVALID)) {
			ast_result = parse_expression(parser, lexer_result);
			if (ast_result.error != AST_NO_ERROR) {
				return ast_result;
			}
		}
		if (IS_CURR_OF_TYPE(lexer_result, TOK_SCOL)) {
			NEXT_TOKEN(lexer_result);
		}
	}

	return ast_result;
}

Ast_Result parse_return_statement(Parser* parser, Token** token) {
	Ast_Result result = parser_create_node(parser, token);
	Ast_Result right_result = parse_expression(parser, token);
	if (right_result.error != AST_NO_ERROR) {
		result.error = right_result.error;
		return result;
	}
	if (!IS_CURR_OF_TYPE(token, TOK_SCOL)) {
		REPORT_ERROR(*(token - 1), "Missing `;`.");
		result.error = AST_ERROR;
		return result;
	} else {
		NEXT_TOKEN(token);
	}

	result.node->right = right_result.node;
	result.node->middle = *(Ast_Node**)FUNCTION_PEEK();

	return result;
}

Ast_Result parse_while_statement(Parser* parser, Token** token) {
	assert((*token)->type == TOK_WHILE);
	Ast_Result ast_result = parser_create_node(parser, token);

	Ast_Result middle_ast_result = parse_expression(parser, token);
	ast_result.node->middle = middle_ast_result.node;

	// @Temporary
	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		REPORT_ERROR(*token, "Expected token `{`");
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	Ast_Result left_ast_result = parse_expression(parser, token);
	if (left_ast_result.error != AST_NO_ERROR) {
		ast_result.error = left_ast_result.error;
		return ast_result;
	}
	ast_result.node->left = left_ast_result.node;

	return ast_result;
}

Ast_Result parse_for_statement(Parser* parser, Token** token) {
	Ast_Result for_statement = parser_create_node(parser, token);
	Ast_Result for_condition = parse_expression(parser, token);
	if (for_statement.error != AST_NO_ERROR) {
		for_statement.error = for_condition.error;
		return for_statement;
	}

	for_statement.node->middle = for_condition.node;

	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		REPORT_ERROR(*token, "Expected token {");
		for_statement.error = AST_ERROR;
		return for_statement;
	}

	Ast_Result for_body = parse_expression(parser, token);
	if (for_body.error != AST_NO_ERROR) {
		for_statement.error = for_body.error;
		return for_statement;
	}

	for_statement.node->right = for_body.node;

	return for_statement;
}

Ast_Result parse_fn_statement(Parser* parser, Token** token) {
	assert(IS_CURR_OF_TYPE(token, TOK_FN));

	Ast_Result fn_result = parser_create_node(parser, token);
	Ast_Result left_result = parser_create_node_no_inc(parser, NEXT_TOKEN(token));
	if (left_result.node->node_type != AST_IDENT) {
		REPORT_ERROR(*token, "Expected an identifier");
		fn_result.error = AST_ERROR;
		return fn_result;
	}

	fn_result.node->left = left_result.node;
	if (!IS_CURR_OF_TYPE(token, TOK_LPAREN)) {
		REPORT_ERROR(*token, "Expected token (");
		fn_result.error = AST_ERROR;
		return fn_result;
	} else {
		NEXT_TOKEN(token); // skip LPAREN
	}

	Ast_Result arg_list_result = parse_argument_list(parser, token);
	if (arg_list_result.error != AST_NO_ERROR) {
		REPORT_ERROR(*token, "Unable to parse argument list");
		fn_result.error = AST_ERROR;
		return fn_result;
	}
	fn_result.node->middle = arg_list_result.node;

	if (IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		NEXT_TOKEN(token);
	}

	if (IS_CURR_OF_TYPE(token, TOK_THIN_ARRW)) {
		Ast_Result ret_type_result = parse_ret_type_node(parser, token);
		if (ret_type_result.error != AST_NO_ERROR) {
			fn_result.error = AST_ERROR;
			return fn_result;
		}
		fn_result.node->ret_type = ret_type_result.node;
	}

	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		REPORT_ERROR(*token, "Missing function body.");
		fn_result.error = AST_ERROR;
		return fn_result;
	}

	FUNCTION_PUSH(&fn_result.node);
	Ast_Result fn_body_result = parse_block_node(parser, token);
	if (fn_body_result.error != AST_NO_ERROR) {
		fn_result.error = AST_ERROR;
		return fn_result;
	}
	fn_result.node->right = fn_body_result.node;
	FUNCTION_POP(NULL);

	return fn_result;
}

Ast_Result parse_ret_type_node(Parser* parser, Token** token) {
	Ast_Result result = parser_create_node(parser, token);
	Ast_Result ret_type_iden = parser_create_node(parser, token);
	if (ret_type_iden.error != AST_NO_ERROR) {
		REPORT_ERROR(*token, "Unable to parse return type.");
		result.error = AST_ERROR;
		return result;
	}
	result.node->right = ret_type_iden.node;
	return result;
}

inline const Type* resolve_pointer_type(Ast_Node* node) {
	const Type* type = NULL;
	if (node->node_type == AST_IDENT) {
		type = resolve_primitive_type(node->token.name);
		if (type) {
			memcpy(&node->type, type, sizeof(Type));
		} else {
			#if LANG_DEBUG
			COLOR(TEXT_YELLOW);
			REPORT_LINE("WARNING: Unresolved type `%s`.", node->token.name);
			CLEAR;
			#endif
		}
	} else if (node->node_type == AST_DEREF || node->node_type == AST_ADDR) {
		Type new_type;
		type = resolve_pointer_type(node->right);
		new_type = *type;
		new_type.flags |= TYPE_POINTER;
		new_type.size = TYPE_LONG_SIZE;
		memcpy(&node->type, &new_type, sizeof(Type));
	}
	return type;
}

Ast_Result parse_type_decl_node(Parser* parser, Token** token) {
	Ast_Result type_decl_result;
	Ast_Result iden_result = parser_create_node_no_inc(parser, NEXT_TOKEN(token));
	if (iden_result.node->node_type != AST_IDENT) {
		REPORT_ERROR(*token, "Token should be of type `identifier`.");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	if (!IS_CURR_OF_TYPE(token, TOK_COL)) {
		REPORT_ERROR(*token, "Expected token `:`.");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	type_decl_result = parser_create_node(parser, token);

	Ast_Result type_result = parse_prefix(parser, token);
	// Check error?
	(void) resolve_pointer_type(type_result.node);
	// Copy the resolved type to the identifier
	iden_result.node->type = type_result.node->type;
	if (IS_CURR_OF_TYPE(token, TOK_LBRACK)) {
		type_result = parse_array_index(parser, token);
	}

	// We don't allow dynamic sizes of static arrays.
	if (type_result.node->node_type == AST_ARRAY_INDEX) {
		if (type_result.node->right->node_type != AST_LITERAL || type_result.node->right->token.type != TOK_LIT_INT) {
			REPORT_ERROR(&type_result.node->token,
						 "Array declaration size must be of type `%s`. Got `%s`.",
						 token_value[TOK_LIT_INT],
						 token_value[type_result.node->right->token.type]);
			type_decl_result.error = AST_ERROR;
			return type_decl_result;
		}

		if (type_result.node->right->token.integer_value < 0) {
			REPORT_ERROR(&type_result.node->token,
						 "Array declaration size be a non-negative number");
			type_decl_result.error = AST_ERROR;
			return type_decl_result;
		}
		// Array index identifier will be the type of the array.
		Type* type = map_get(&parser->symbols, type_result.node->left->token.name);

		iden_result.node->type = type_result.node->type = *type;
		iden_result.node->type.flags |= TYPE_ARRAY;
		type_result.node->type.flags |= TYPE_ARRAY;
		// Set element count of the identifier to the element count of
		// the statically declared array.
		iden_result.node->type.elements = type_result.node->right->token.integer_value;
	}

	if (type_result.error != AST_NO_ERROR) {
			type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	if ((type_result.node->node_type != AST_IDENT && type_result.node->node_type != AST_ARRAY_INDEX)
		&& type_result.node->node_type != AST_DEREF
		&& type_result.node->node_type != AST_ADDR) {
		REPORT_ERROR(*token, "Token should be of type `identifier`.");
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}

	type_decl_result.node->left = iden_result.node;
	type_decl_result.node->right = type_result.node;

	#if PARSER_DEFINED_CHECK
	if (parser_is_defined(parser, type_decl_result.node->left->token.name)) {
		REPORT_ERROR(&type_decl_result.node->left->token,
							"Identifier `%s` is already defined.",
							type_decl_result.node->left->token.name);
		type_decl_result.error = AST_ERROR;
		return type_decl_result;
	}
	#endif

	// After resolving the type. We put it in the variable map.
	map_put(&parser->symbols, iden_result.node->token.name, &iden_result.node->type);
	map_put(&parser_peek_scope(parser)->variables,
			  type_decl_result.node->left->token.name,
			  &type_decl_result.node->right->type);

	PARSER_PUSH(&type_decl_result);

	return type_decl_result;
}

Ast_Result parse_if_statement(Parser* parser, Token** token) {
	assert((*token)->type == TOK_IF);
	Ast_Result ast_result = parser_create_node(parser, token);

	Ast_Result middle_ast_result = parse_expression(parser, token);
	if (middle_ast_result.error != AST_NO_ERROR) {
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	ast_result.node->middle = middle_ast_result.node;

	// @Temporary
	if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		REPORT_ERROR(*token, "Expected token `{`.");
		ast_result.error = AST_ERROR;
		return ast_result;
	}

	Ast_Result left_ast_result = parse_expression(parser, token);
	if (left_ast_result.error != AST_NO_ERROR) {
		ast_result.error = left_ast_result.error;
		return ast_result;
	}
	ast_result.node->left = left_ast_result.node;

	if (IS_CURR_OF_TYPE(token, TOK_ELSE)) {
		NEXT_TOKEN(token); // skip TOK_ELSE

		// @Temporary
		if (!IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
			REPORT_ERROR(*token, "Expected token `{`.");
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

	parser_push_scope(parser);
	while (!IS_CURR_OF_TYPE(token, TOK_RBRACE)) {
		Ast_Result node_result = parse_statement(parser, token);
		if (node_result.error != AST_NO_ERROR) {
			ast_result.error = node_result.error;
			break;
		}
		ast_block_node_add_node(ast_result.node, node_result.node);
	}
	NEXT_TOKEN(token); // skip TOK_RBRACE
	parser_pop_scope(parser, NULL);

	return ast_result;
}


Ast_Result parse_suffix(Parser* parser, Token** token) {
	Ast_Result left_ast_result = parser_create_node(parser, token);
	if (left_ast_result.error != AST_NO_ERROR) {
		left_ast_result.error = AST_ERROR;
		return left_ast_result;
	}

	// if (IS_CURR_OF_TYPE(token, TOK_INC)
	// 	|| IS_CURR_OF_TYPE(token, TOK_DEC)) {
		Ast_Result suffix_result = parser_create_node(parser, token);
		if (suffix_result.error != AST_NO_ERROR) {
			left_ast_result.error = AST_ERROR;
			return left_ast_result;
		}

		// parser_create_node is dumb, and it will always create a node with the
		// type of the pre inc/dec.
		if (suffix_result.node->node_type == AST_PREDEC) {
			suffix_result.node->node_type = AST_POSTDEC;
		} else if (suffix_result.node->node_type == AST_PREINC) {
			suffix_result.node->node_type = AST_POSTINC;
		}

		suffix_result.node->right = left_ast_result.node;

		PARSER_PUSH(&suffix_result);

		return suffix_result;

	// } else {
	// 	return ast_result;
	// }
}

Ast_Result parse_prefix(Parser* parser, Token** token) {
	Token* curr_tok = *token;
	Ast_Result ast_result = parser_create_node(parser, token);
	if (ast_result.error != AST_NO_ERROR) {
		return ast_result;
	}
	if (curr_tok->type == TOK_STAR) {
		ast_result.node->node_type = AST_DEREF;
	} else if (curr_tok->type == TOK_AMP) {
		ast_result.node->node_type = AST_ADDR;
	} else if (ast_result.node->node_type != AST_PREINC
			   && ast_result.node->node_type != AST_PREDEC) {
		return ast_result;
	}

	Ast_Result ast_right_result = parse_prefix(parser, token);
	if (ast_right_result.error != AST_NO_ERROR) {
		ast_result.error = AST_ERROR;
		return ast_result;
	}
	ast_result.node->right = ast_right_result.node;

	PARSER_PUSH(&ast_result);

	return ast_result;
}

Ast_Result parse_expression(Parser* parser, Token** token) {
	Ast_Result ast_result = {.error = AST_NO_ERROR, 0};

	// FIRST STEP

	// @Temporary if the previous token is an in identifier that means that
	// we're parsing a function call
	if (IS_CURR_OF_TYPE(token, TOK_LPAREN) &&
		!IS_PREV_OF_TYPE(token, TOK_IDEN)) {
		NEXT_TOKEN(token);
		ast_result = parse_expression(parser, token);
		if (ast_result.error != AST_NO_ERROR) {
			return ast_result;
		}
		ast_result.node->precedence = S32_MAX;

		if (!IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
			REPORT_ERROR(*token, "Expected token `)`.");
			ast_result.error = AST_ERROR;
			return ast_result;
		}

		PARSER_PUSH(&ast_result);

		Ast_Result new = parse_expression(parser, token);
		if (new.error != AST_NO_ERROR) {
			return new;
		}

		if (new.node == NULL)
			return ast_result;

		new.node = fix_precedence(new.node);
		PARSER_PUSH(&new);
		return new;
	} else if (IS_CURR_OF_TYPE(token, TOK_STAR)
			   || IS_CURR_OF_TYPE(token, TOK_AMP)) {
		ast_result = parse_prefix(parser, token);
		// @Warning this way of parsing will break future binary AND operator.
		// @Note Safety check. We don't allow referencing non-pointer types.
		#if PARSER_DEFINED_CHECK
		if (ast_result.node->node_type == AST_DEREF) {
			if (!(ast_result.node->right->type.flags & TYPE_POINTER)) {
				REPORT_ERROR(&ast_result.node->token, "Cannot dereference a non-pointer type.");
				ast_result.error = AST_ERROR;
				return ast_result;
			}
		}
		#endif
	} else if (IS_CURR_OF_TYPE(token, TOK_IDEN)
			   && IS_PEEK_OF_TYPE(token, TOK_COL)) {
		ast_result = parse_type_decl_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_IDEN)
			   && IS_PEEK_OF_TYPE(token, TOK_IDEN)) {
		// Triggered in case were we have two consecutive identifiers which is
		// in no situation a valid expression.
		const char* value = token_value[(*(token) + 1)->type];
		REPORT_ERROR(*(token) + 1, "Unexpected token `%s`.",
					 value ? value : (*(token) + 1)->name);
		ast_result.error = AST_ERROR;
		return ast_result;
	} else if (IS_CURR_OF_TYPE(token, TOK_LBRACE)) {
		return parse_block_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_RBRACE)
			   || IS_CURR_OF_TYPE(token, TOK_COMMA)) {
		const char* value = token_value[(*token)->type];
		REPORT_ERROR(*token, "Unexpected token `%s`.",
					 value ? value : (*token)->name);
		ast_result.error = AST_ERROR;
		return ast_result;
	} else if (IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		NEXT_TOKEN(token);
	} else if (IS_CURR_OF_TYPE(token, TOK_IDEN)
			   && IS_PEEK_OF_TYPE(token, TOK_LPAREN)) {
		// @Temporary because we can link against C and call its functions
		// we can allow undefined identifiers for function calls.
		parser_create_node_no_inc(parser, NEXT_TOKEN(token));
		ast_result = parse_function_call(parser, token);
	} else if (
		IS_CURR_OF_TYPE(token, TOK_LIT_INT)
		|| IS_CURR_OF_TYPE(token, TOK_LIT_FLT)
		|| IS_CURR_OF_TYPE(token, TOK_LIT_CHR)
		|| IS_CURR_OF_TYPE(token, TOK_LIT_STR)
		|| IS_CURR_OF_TYPE(token, TOK_IDEN)
		|| IS_CURR_OF_TYPE(token, TOK_TRUE)
		|| IS_CURR_OF_TYPE(token, TOK_FALSE)
		|| IS_CURR_OF_TYPE(token, TOK_NULL)
		|| IS_CURR_OF_TYPE(token, TOK_INC)
		|| IS_CURR_OF_TYPE(token, TOK_DEC)
		) {
		ast_result = parse_prefix(parser, token);
		// ast_result = parser_create_node(parser, token);
	}

	// If there was an error in the first part of the parsing
	// there is no reason for continuing.
	if (ast_result.error != AST_NO_ERROR) {
		return ast_result;
	}

	// SECOND STEP

	if (IS_CURR_OF_TYPE(token, TOK_LPAREN)) {
		NEXT_TOKEN(token); // skip LPAREN
		ast_result = parse_argument_list(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_ASSN)) {
		ast_result = parse_assignment_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_LBRACK)) {
		ast_result = parse_array_index(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_IN)) {
		ast_result = parse_in_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_DDOT)) {
		ast_result = parse_iter_node(parser, token);
	} else if (IS_CURR_OF_TYPE(token, TOK_AND) ||
			   IS_CURR_OF_TYPE(token, TOK_OR)) {
		ast_result = parse_boolean_node(parser, token);
		// @formatter:off
	} else if (IS_CURR_OF_TYPE(token, TOK_EQ)   ||
			   IS_CURR_OF_TYPE(token, TOK_NE)   ||
			   IS_CURR_OF_TYPE(token, TOK_GT)   ||
			   IS_CURR_OF_TYPE(token, TOK_LT)   ||
			   IS_CURR_OF_TYPE(token, TOK_GE)   ||
			   IS_CURR_OF_TYPE(token, TOK_LE)   ||
			   IS_CURR_OF_TYPE(token, TOK_ADD)  ||
			   IS_CURR_OF_TYPE(token, TOK_SUB)  ||
			   IS_CURR_OF_TYPE(token, TOK_STAR) ||
			   IS_CURR_OF_TYPE(token, TOK_DIV)  ||
			   IS_CURR_OF_TYPE(token, TOK_MOD)) {
		// @formatter:on
		// All of these are for now parsed as a generic binary operation.
		ast_result = parse_binary_operation_node(parser, token);
		// @formatter:on
	} else if (IS_CURR_OF_TYPE(token, TOK_SUBASSN) ||
			   IS_CURR_OF_TYPE(token, TOK_ADDASSN) ||
			   IS_CURR_OF_TYPE(token, TOK_MULASSN) ||
			   IS_CURR_OF_TYPE(token, TOK_DIVASSN)) {
		// @formatter:on
		ast_result = parse_assn_binop_node(parser, token);
	}

	return ast_result;
}

Ast_Result parse_array_index(Parser* parser, Token** token) {
	Ast_Result result;
	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		REPORT_ERROR(*token, "Unable to array index.");
		result.error = AST_ERROR;
		return result;
	} else {
		PARSER_POP(&left_ast_result);
	}
	result = parser_create_node(parser, token);

	result.node->left = left_ast_result.node;

	Ast_Result index_result = parse_expression(parser, token);
	if (index_result.error != AST_NO_ERROR) {
		result.error = result.error;
		return result;
	}

	result.node->right = index_result.node;

	if (!IS_CURR_OF_TYPE(token, TOK_RBRACK)) {
		REPORT_ERROR(*token, "Expected token ]");
		result.error = AST_ERROR;
		return result;
	} else {
		NEXT_TOKEN(token);
	}

	PARSER_PUSH(&result);

	return result;
}

Ast_Result parse_iter_node(Parser* parser, Token** token) {
	Ast_Result iter_node;
	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		REPORT_ERROR(*token, "Unable to parse iterator.");
		iter_node.error = AST_ERROR;
		return iter_node;
	} else {
		PARSER_POP(&left_ast_result);
	}
	iter_node = parser_create_node_no_inc(parser, NEXT_TOKEN(token));
	if (iter_node.error != AST_NO_ERROR) {
		iter_node.error = iter_node.error;
		return iter_node;
	}

	iter_node.node->left = left_ast_result.node;

	Ast_Result right_ast_result = parse_expression(parser, token);
	if (right_ast_result.error != AST_NO_ERROR) {
		iter_node.error = right_ast_result.error;
		return iter_node;
	}

	iter_node.node->right = right_ast_result.node;


	return iter_node;
}

Ast_Result parse_in_node(Parser* parser, Token** token) {
	Ast_Result in_node;
	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		REPORT_ERROR(*token, "Unable to parse `in` node.");
		in_node.error = AST_ERROR;
		return in_node;
	} else {
		PARSER_POP(&left_ast_result);
	}
	in_node = parser_create_node(parser, token);

	in_node.node->left = left_ast_result.node;

	Ast_Result right_ast_result = parse_expression(parser, token);
	if (right_ast_result.error != AST_NO_ERROR) {
		in_node.error = right_ast_result.error;
		return in_node;
	}

	in_node.node->right = right_ast_result.node;

	return in_node;
}

Ast_Result parse_function_call(Parser* parser, Token** token) {
	Ast_Result result;
	// @Incomplete parse TOK_DOT
	Ast_Result left_ast_result;
	if (stack_is_empty(&parser->node_stack)) {
		REPORT_ERROR(*token, "Unable to parse function call.");
		result.error = AST_ERROR;
		return result;
	} else {
		PARSER_POP(&left_ast_result);
	}

	// @Temporary create a fake lparen token
	Token lparen_token = **token;
	lparen_token.type = TOK_LPAREN;
	result = parser_create_node_no_inc(parser, &lparen_token);
	result.node->node_type = AST_FUNC_CALL;

	result.node->left = left_ast_result.node;

	if (!IS_CURR_OF_TYPE(token, TOK_LPAREN)) {
		REPORT_ERROR(*token, "Expected token `(`.");
		result.error = AST_ERROR;
		return result;
	} else {
		NEXT_TOKEN(token); // skip LPAREN
	}

	Ast_Result arg_list_result = parse_argument_list(parser, token);
	if (arg_list_result.error != AST_NO_ERROR) {
		result.error = AST_ERROR;
		return result;
	}

	result.node->middle = arg_list_result.node;

	PARSER_PUSH(&result);

	return result;
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

	if (left_ast_result.node->node_type == AST_ADDR) {
		REPORT_ERROR(&left_ast_result.node->token, "Cannot take an address of an lvalue identifier.");
		result.error = AST_ERROR;
		return result;
	}

	Ast_Result right_ast_result = parse_expression(parser, token);
	if (right_ast_result.error != AST_NO_ERROR) {
		result.error = right_ast_result.error;
		return result;
	}

	result.node->left = left_ast_result.node;
	result.node->right = right_ast_result.node;

	#if PARSER_DEFINED_CHECK
	if (result.node->left->token.type == TOK_IDEN
		&& !parser_is_defined(parser, result.node->left->token.name)) {
		REPORT_ERROR(&result.node->left->token,
							"Undeclared identifier `%s`.",
							result.node->left->token.name);
		result.error = AST_ERROR;
		return result;
	}
	#endif

	#if PARSER_DEFINED_CHECK
	if (result.node->right->token.type == TOK_IDEN
		&& !parser_is_defined(parser, result.node->right->token.name)) {
		REPORT_ERROR(&result.node->left->token,
							"Undeclared identifier `%s`.",
							result.node->right->token.name);
		result.error = AST_ERROR;
		return result;
	}
	#endif

	// If the assignment is happening between a type decl that is not defining
	// a pointer and an address node we fail.
	if (result.node->left->node_type == AST_TYPE_DECL) {
		if (!(result.node->left->right->type.flags & TYPE_POINTER) == (result.node->right->node_type == AST_ADDR)) {
			REPORT_ERROR(&result.node->token, "Cannot assign an address to a non-pointer type.");
			result.error = AST_ERROR;
			return result;
		}
	}

	if (result.node->left != NULL &&
		result.node->left->node_type == AST_IDENT) {
		result.node->left->node_type = AST_LVIDENT;
	}

	if (result.node->left != NULL &&
		result.node->left->node_type == AST_TYPE_DECL) {
		result.node->left->left->node_type = AST_LVIDENT;
	}

	// Fix type for references
	if (result.node->left->node_type == AST_DEREF) {
		Ast_Node* iden = result.node->left->right;
		while (iden != NULL && iden->node_type == AST_DEREF)
			iden = iden->right;
		if (iden != NULL)
			iden->node_type = AST_LVIDENT;
	}

	PARSER_PUSH(&result);

	return result;
}

Ast_Result parse_argument_list(Parser* parser, Token** token) {
	Ast_Result argument_list_result;
	// @Temporary create a fake block token
	Token block_token = **token;
	block_token.type = TOK_LBRACE;

	argument_list_result = parser_create_node_no_inc(parser, &block_token);
	// @Temporary
	argument_list_result.node->node_type = AST_ARG_LIST;

	// Parse all the come delimited expressions as argument nodes.
	while (!IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		Ast_Result argument_result = parse_expression(parser, token);
		if (argument_result.error != AST_NO_ERROR) {
			argument_list_result.error = AST_ERROR;
			return argument_list_result;
		}
		ast_block_node_add_node(argument_list_result.node,
								argument_result.node);
		if (!IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
			assert(IS_CURR_OF_TYPE(token, TOK_COMMA));
			NEXT_TOKEN(token);
		}
	}

	// If the argument list is empty tokens will not be incremented in the
	// parsing while loop.
	if (IS_CURR_OF_TYPE(token, TOK_RPAREN)) {
		NEXT_TOKEN(token);
	}

	return argument_list_result;
}

void parser_report_error(Parser* parser, Token* token, const char* format, ...) {
	Parser_Error_Report error_report;
	va_list args;
	va_start (args, format);
	vsnprintf(error_report.text, PARSER_ERROR_BUFLEN, format, args);
	va_end(args);
	error_report.source = token;
	list_push(&parser->errors, &error_report);
}

inline void parser_new(Parser* parser, char* code) {
	parser->code.size = strlen(code);
	parser->code.text = code;
	list_new(&parser->errors, sizeof(Parser_Error_Report));
	stack_new(&parser->node_stack, sizeof(Ast_Result));
	stack_new(&parser->function_stack, sizeof(Ast_Node*));
	stack_new(&parser->scopes, sizeof(Scope));
	map_new(&parser->symbols, sizeof(Type));
	for (int i = 1; i < PRIMITIVE_TYPES_LEN; ++i) {
		Type* type = &primitive_types[i];
		map_put(&parser->symbols, type->name, type);
	}
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
		.node = 0
	};

	// For identifiers, we check whether they are defined.
	#if PARSER_DEFINED_CHECK
	if ((*token)->type == TOK_IDEN && !parser_is_defined(parser, (*token)->name)) {
		REPORT_ERROR(*token, "Undefined identifier `%s`.",
							(*token)->name);
		ast_result.error = AST_ERROR;
		return ast_result;
	}
	#endif

	ast_result.node= ast_node_new(NEXT_TOKEN(token));

	#if PARSER_DEFINED_CHECK
	// It is defined for sure. We now need to update its type.
	if (ast_result.node->node_type == AST_IDENT || ast_result.node->node_type == AST_LVIDENT) {
		Type* type = map_get(&parser->symbols, ast_result.node->token.name);
		ast_result.node->type = *type;
	}
	#endif

	stack_push(&parser->node_stack, &ast_result);
	return ast_result;
}

Ast_Result parser_create_node_no_inc(Parser* parser, Token* token) {
	Ast_Result ast_result = {
		.error= AST_NO_ERROR,
		.node= ast_node_new(token)
	};

	stack_push(&parser->node_stack, &ast_result);
	return ast_result;
}

inline Scope* parser_peek_scope(Parser* parser) {
	return (Scope*) stack_peek(&parser->scopes);
}

inline Scope* parser_push_scope(Parser* parser) {
	Scope scope = {0};
	map_new(&scope.variables, sizeof(char*));
	return (Scope*) stack_push(&parser->scopes, &scope);
}

inline bool parser_pop_scope(Parser* parser, Scope* dest) {
	return stack_pop(&parser->scopes, dest);
}

bool parser_is_defined(Parser* parser, const char* var_name) {
	for (s32 i = (s32) parser->scopes.count - 1; i >= 0; --i) {
		Scope* scope = (Scope*) (parser->scopes.base + (i * parser->scopes.size));
		if (map_get(&scope->variables, var_name) != NULL) {
			return true;
		}
	}
	// If the variable wasn't found. Check if there's a type defined
	if (map_get(&parser->symbols, var_name) != NULL) return true;
	return false;
}


#pragma clang diagnostic pop