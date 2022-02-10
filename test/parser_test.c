#include "parser.h"

inline static Parser_Result PARSER_TEST_CASE(char* code) {
	Parser parser;
	Lexer lexer;
	lexer_new(&lexer, code);
	lexer_lex(&lexer);
	parser_new(&parser, code);
	printf("CODE: %s\n", code);
	list_foreach(&lexer.tokens, Token*, {
		printf("%s\n", token_repr[it->type]);
	})
	Parser_Result retval = parser_parse(&parser, &lexer);
	lexer_free(&lexer);
	parser_free(&parser);
	return retval;
}

int main(void) {

	Parser_Result result;
	Ast_Node* root = NULL;

	result = PARSER_TEST_CASE("if true {1;} else {2;}");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_IF);
	assert(root->middle->node_type == AST_LITERAL);
	assert(root->left->node_type == AST_BLOCK);
	assert(list_get_as_deref(&root->left->nodes, 0, Ast_Node*)->node_type == AST_LITERAL);
	assert(root->right->node_type == AST_BLOCK);
	assert(list_get_as_deref(&root->right->nodes, 0, Ast_Node*)->node_type == AST_LITERAL);


	result = PARSER_TEST_CASE("b:s32=0; if a == true {b=2;} else {b=3;}");
	root = *(Ast_Node**) list_get(&result.nodes, 1);
	assert(root->node_type == AST_IF);
	assert(root->middle->node_type == AST_EQUALITY);
	assert(root->middle->left->node_type == AST_IDENT);
	assert(root->middle->right->node_type == AST_LITERAL);
	assert(root->left->node_type == AST_BLOCK);
	assert(list_get_as(&root->left->nodes, 0, Ast_Node*)->node_type == AST_ASSIGN);
	assert(root->right->node_type == AST_BLOCK);
	assert(list_get_as(&root->left->nodes, 0, Ast_Node*)->node_type == AST_ASSIGN);

	result = PARSER_TEST_CASE("a == true { function; }}");
	assert(result.errors.count >= 1);

	result = PARSER_TEST_CASE("a:s32=0;b:s32=0;c:s32=0;d:s32=0;\n"
							  "if a == b || b > c && c < d {\n"
							  "  print(a,b,c);\n"
							  "  print(\"Hello world!\");\n"
							  "  b = `David je najjaci!`;\n"
							  "} else {\n"
							  "  a = 42;\n"
							  "  b = 42;\n"
							  "  c = 42;\n"
							  "  d = 42;\n"
							  "}");
	root = *(Ast_Node**) list_get(&result.nodes, 4);
	assert(result.errors.count == 0);
	assert(root->node_type == AST_IF);
	assert(root->middle->node_type == AST_BOOLEAN);

	result = PARSER_TEST_CASE("if (a == b) || b {"
							  "} else {"
							  "}");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_IF);
	assert(root->middle->node_type == AST_BOOLEAN);
	assert(root->middle->left->node_type == AST_EQUALITY);
	assert(root->middle->right->node_type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);

	result = PARSER_TEST_CASE("if (a == (b)) || b {"
							  "} else {"
							  "}");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_IF);
	assert(root->middle->node_type == AST_BOOLEAN);
	assert(root->middle->left->node_type == AST_EQUALITY);
	assert(root->middle->left->left->node_type == AST_IDENT);
	assert(root->middle->left->right->node_type == AST_IDENT);
	assert(root->middle->right->node_type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);


	result = PARSER_TEST_CASE("(1 + 2) * 3 + 5");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->node_type == AST_ARITHMETIC);
	assert(root->token.type == TOK_ADD);
	assert(root->left->node_type == AST_ARITHMETIC);
	assert(root->right->node_type == AST_LITERAL);

	result = PARSER_TEST_CASE("str: string = \"test\";");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->node_type == AST_TYPE_DECL);
	assert(root->right->node_type == AST_LITERAL);

	result = PARSER_TEST_CASE("a:s32 = 123;");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->node_type == AST_TYPE_DECL);
	assert(root->left->left->node_type == AST_LVIDENT);
	assert(root->right->node_type == AST_LITERAL);

	result = PARSER_TEST_CASE("a:s32=0;b:s32=0;a = (a + b) - 1; a = a + b;");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 2);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->node_type == AST_LVIDENT);
	assert(root->right->node_type == AST_ARITHMETIC);
	assert(root->right->token.type == TOK_SUB);
	assert(root->right->left->node_type == AST_ARITHMETIC);
	assert(root->right->left->token.type == TOK_ADD);
	assert(root->right->right->node_type == AST_LITERAL);
	root = *(Ast_Node**) list_get(&result.nodes, 3);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->node_type == AST_LVIDENT);
	assert(root->right->node_type == AST_ARITHMETIC);
	assert(root->right->left->node_type == AST_IDENT);
	assert(root->right->right->node_type == AST_IDENT);
	assert(strcmp(root->right->left->token.name, "a") == 0);
	assert(strcmp(root->right->right->token.name, "b") == 0);

	result = PARSER_TEST_CASE("fn write(file: File, count: u64, byte: u32) -> void {print();}");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_FUNC_DEF);
	assert(root->left->node_type == AST_IDENT);
	assert(root->middle->node_type == AST_ARG_LIST);
	assert(list_get_as_deref(&root->middle->nodes, 0, Ast_Node*)->node_type == AST_TYPE_DECL);
	assert(list_get_as_deref(&root->middle->nodes, 1, Ast_Node*)->node_type == AST_TYPE_DECL);
	assert(list_get_as_deref(&root->middle->nodes, 2, Ast_Node*)->node_type == AST_TYPE_DECL);
	assert(root->right->node_type == AST_BLOCK);
	assert(list_get_as_deref(&root->right->nodes, 0, Ast_Node*)->node_type == AST_FUNC_CALL);
	assert(list_get_as_deref(&root->right->nodes, 0, Ast_Node*)->middle->nodes.count == 0);
	assert(root->ret_type->node_type == AST_FUNC_RET_TYPE);

	result = PARSER_TEST_CASE("write(,,);");
	assert(result.errors.count == 1);

	result = PARSER_TEST_CASE("for name in names { print(name); }");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_FOR);
	assert(root->middle->node_type == AST_IN);
	assert(root->middle->left->node_type == AST_IDENT);
	assert(root->middle->right->node_type == AST_IDENT);
	assert(root->right->node_type == AST_BLOCK);

	result = PARSER_TEST_CASE("for i in 1..10 { print(i); }");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_FOR);
	assert(root->middle->node_type == AST_IN);
	assert(root->middle->left->node_type == AST_IDENT);
	assert(root->middle->right->node_type == AST_ITER);
	assert(root->middle->right->left->node_type == AST_LITERAL);
	assert(root->middle->right->right->node_type == AST_LITERAL);
	assert(root->right->node_type == AST_BLOCK);

	result = PARSER_TEST_CASE("a:*s32 = &b;");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_ASSIGN);
	assert(root->left->node_type == AST_TYPE_DECL);
	assert(root->left->right->node_type == AST_DEREF);

	result = PARSER_TEST_CASE("*c = 10;");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->node_type == AST_ASSIGN);
	assert(root->left->node_type == AST_DEREF);
	assert(root->right->node_type == AST_LITERAL);

	result = PARSER_TEST_CASE("fn write(file: File, count: u64, byte: u32) -> void {"
							  "a:s32=1;a:s32=1;"
							  "}");
	assert(result.errors.count == 1);
}