#include "parser.h"

int main(void) {

	Parser_Result result;
	Ast_Node* first_node = NULL;
	Parser parser;
	Lexer lexer;

	char* code1 = "if true {1} else {2};";
	lexer_new(&lexer, code1);
	lexer_lex(&lexer);
	parser_new(&parser, code1);
	result = parser_parse(&parser, &lexer);

	first_node = result.nodes[0];
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_LITERAL);
	assert(first_node->left->type == AST_BLOCK);
	assert(first_node->left->nodes[0]->type == AST_LITERAL);
	assert(first_node->right->type == AST_BLOCK);
	assert(first_node->right->nodes[0]->type == AST_LITERAL);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code2 = "if a == true {2} else {3};";
	lexer_new(&lexer, code2);
	lexer_lex(&lexer);
	parser_new(&parser, code2);
	result = parser_parse(&parser, &lexer);

	first_node = result.nodes[0];
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_EQUALITY);
	assert(first_node->middle->left->type == AST_IDENT);
	assert(first_node->middle->right->type == AST_LITERAL);
	assert(first_node->left->type == AST_BLOCK);
	assert(first_node->left->nodes[0]->type == AST_LITERAL);
	assert(first_node->right->type == AST_BLOCK);
	assert(first_node->right->nodes[0]->type == AST_LITERAL);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code3 = "a == true { function; }}";
	lexer_new(&lexer, code3);
	lexer_lex(&lexer);
	parser_new(&parser, code3);
	result = parser_parse(&parser, &lexer);
	assert(result.error.count == 1);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code4 = "if a == b || b > c && c < d {"
				  "	print(a,b,c);"
				  "	print(\"Hello world!\");"
				  "	b = `David je najjaci!`"
				  "} else {"
				  " a = 42;"
				  " b = 42;"
				  " c = 42;"
				  " d = 42;"
				  "}";
	lexer_new(&lexer, code4);
	lexer_lex(&lexer);
	parser_new(&parser, code4);
	result = parser_parse(&parser, &lexer);
	first_node = result.nodes[0];
	assert(result.error.count == 0);
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_BOOLEAN);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code5 = "if (a == b) || b {"
				  "} else {"
				  "}";
	lexer_new(&lexer, code5);
	lexer_lex(&lexer);
	parser_new(&parser, code5);
	result = parser_parse(&parser, &lexer);
	first_node = result.nodes[0];
	assert(result.error.count == 0);
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_BOOLEAN);
	assert(first_node->middle->left->type == AST_EQUALITY);
	assert(first_node->middle->right->type == AST_IDENT);
	assert(first_node->middle->token.type == TOK_OR);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code6 = "if (a == (b)) || b {"
				  "} else {"
				  "}";
	lexer_new(&lexer, code6);
	lexer_lex(&lexer);
	parser_new(&parser, code6);
	result = parser_parse(&parser, &lexer);
	first_node = result.nodes[0];
	assert(result.error.count == 0);
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_BOOLEAN);
	assert(first_node->middle->left->type == AST_EQUALITY);
	assert(first_node->middle->left->left->type == AST_IDENT);
	assert(first_node->middle->left->right->type == AST_IDENT);
	assert(first_node->middle->right->type == AST_IDENT);
	assert(first_node->middle->token.type == TOK_OR);
	lexer_free(&lexer);
	parser_free(&parser);

	char* code7 = "(1 + 2) * 3 + 5";
	lexer_new(&lexer, code7);
	lexer_lex(&lexer);
	parser_new(&parser, code7);
	result = parser_parse(&parser, &lexer);
	first_node = result.nodes[0];
	assert(result.error.count == 0);
	assert(first_node->type == AST_ARITHMETIC);
	assert(first_node->token.type == TOK_ADD);
	assert(first_node->left->type == AST_ARITHMETIC);
	assert(first_node->right->type == AST_LITERAL);
	lexer_free(&lexer);
	parser_free(&parser);
}