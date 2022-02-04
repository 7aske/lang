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

	result = PARSER_TEST_CASE("if true {1} else {2};");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_LITERAL);
	assert(root->left->type == AST_BLOCK);
	assert(root->left->nodes[0]->type == AST_LITERAL);
	assert(root->right->type == AST_BLOCK);
	assert(root->right->nodes[0]->type == AST_LITERAL);


	result = PARSER_TEST_CASE("if a == true {2} else {3};");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_EQUALITY);
	assert(root->middle->left->type == AST_IDENT);
	assert(root->middle->right->type == AST_LITERAL);
	assert(root->left->type == AST_BLOCK);
	assert(root->left->nodes[0]->type == AST_LITERAL);
	assert(root->right->type == AST_BLOCK);
	assert(root->right->nodes[0]->type == AST_LITERAL);

	result = PARSER_TEST_CASE("a == true { function; }}");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 1);

	result = PARSER_TEST_CASE("if a == b || b > c && c < d {"
							  "	print(a,b,c);"
							  "	print(\"Hello world!\");"
							  "	b = `David je najjaci!`"
							  "} else {"
							  " a = 42;"
							  " b = 42;"
							  " c = 42;"
							  " d = 42;"
							  "}");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);

	result = PARSER_TEST_CASE("if (a == b) || b {"
							  "} else {"
							  "}");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);
	assert(root->middle->left->type == AST_EQUALITY);
	assert(root->middle->right->type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);

	result = PARSER_TEST_CASE("if (a == (b)) || b {"
							  "} else {"
							  "}");
	assert(result.errors.count == 0);
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);
	assert(root->middle->left->type == AST_EQUALITY);
	assert(root->middle->left->left->type == AST_IDENT);
	assert(root->middle->left->right->type == AST_IDENT);
	assert(root->middle->right->type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);


	result = PARSER_TEST_CASE("(1 + 2) * 3 + 5");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->type == AST_ARITHMETIC);
	assert(root->token.type == TOK_ADD);
	assert(root->left->type == AST_ARITHMETIC);
	assert(root->right->type == AST_LITERAL);

	result = PARSER_TEST_CASE("str: string = \"test\";");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->type == AST_TYPE_DECL);
	assert(root->right->type == AST_LITERAL);

	result = PARSER_TEST_CASE("a = 123;");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->type == AST_IDENT);
	assert(root->right->type == AST_LITERAL);

	result = PARSER_TEST_CASE("a = (a + b) - 1; a = a + b;");
	root = *(Ast_Node**) list_get(&result.nodes, 0);
	assert(result.errors.count == 0);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->type == AST_IDENT);
	assert(root->right->type == AST_ARITHMETIC);
	assert(root->right->token.type == TOK_SUB);
	assert(root->right->left->type == AST_ARITHMETIC);
	assert(root->right->left->token.type == TOK_ADD);
	assert(root->right->right->type == AST_LITERAL);
	root = *(Ast_Node**) list_get(&result.nodes, 1);
	assert(root->token.type == TOK_ASSN);
	assert(root->left->type == AST_IDENT);
	assert(root->right->type == AST_ARITHMETIC);
	assert(root->right->left->type == AST_IDENT);
	assert(root->right->right->type == AST_IDENT);
	assert(strcmp(root->right->left->token.string_value.data, "a") == 0);
	assert(strcmp(root->right->right->token.string_value.data, "b") == 0);
}