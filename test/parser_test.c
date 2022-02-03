#include "parser.h"

#define PARSER_TEST_CASE(__code) \
code = __code;/*global variable*/\
lexer_new(&lexer,code);\
lexer_lex(&lexer);\
parser_new(&parser, __code);\
result = parser_parse(&parser, &lexer);\
root = *(Ast_Node**)list_get(&result.nodes, 0);

#define PARSER_TEST_FREE()\
lexer_free(&lexer);\
parser_free(&parser);

static char* code;

int main(void) {

	Ast_Node* root = NULL;
	Parser_Result result;
	Parser parser;
	Lexer lexer;

	PARSER_TEST_CASE("if true {1} else {2};")
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_LITERAL);
	assert(root->left->type == AST_BLOCK);
	assert(root->left->nodes[0]->type == AST_LITERAL);
	assert(root->right->type == AST_BLOCK);
	assert(root->right->nodes[0]->type == AST_LITERAL);
	PARSER_TEST_FREE()


	PARSER_TEST_CASE("if a == true {2} else {3};")
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_EQUALITY);
	assert(root->middle->left->type == AST_IDENT);
	assert(root->middle->right->type == AST_LITERAL);
	assert(root->left->type == AST_BLOCK);
	assert(root->left->nodes[0]->type == AST_LITERAL);
	assert(root->right->type == AST_BLOCK);
	assert(root->right->nodes[0]->type == AST_LITERAL);
	PARSER_TEST_FREE()

	PARSER_TEST_CASE("a == true { function; }}")
	assert(result.errors.count == 1);
	PARSER_TEST_FREE()

	PARSER_TEST_CASE("if a == b || b > c && c < d {"
					 "	print(a,b,c);"
					 "	print(\"Hello world!\");"
					 "	b = `David je najjaci!`"
					 "} else {"
					 " a = 42;"
					 " b = 42;"
					 " c = 42;"
					 " d = 42;"
					 "}")
	assert(result.errors.count == 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);
	PARSER_TEST_FREE();

	PARSER_TEST_CASE("if (a == b) || b {"
					 "} else {"
					 "}")
	assert(result.errors.count == 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);
	assert(root->middle->left->type == AST_EQUALITY);
	assert(root->middle->right->type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);
	PARSER_TEST_FREE()

	PARSER_TEST_CASE("if (a == (b)) || b {"
					 "} else {"
					 "}")
	assert(result.errors.count == 0);
	assert(root->type == AST_IF);
	assert(root->middle->type == AST_BOOLEAN);
	assert(root->middle->left->type == AST_EQUALITY);
	assert(root->middle->left->left->type == AST_IDENT);
	assert(root->middle->left->right->type == AST_IDENT);
	assert(root->middle->right->type == AST_IDENT);
	assert(root->middle->token.type == TOK_OR);
	PARSER_TEST_FREE()

	PARSER_TEST_CASE("(1 + 2) * 3 + 5")
	assert(result.errors.count == 0);
	assert(root->type == AST_ARITHMETIC);
	assert(root->token.type == TOK_ADD);
	assert(root->left->type == AST_ARITHMETIC);
	assert(root->right->type == AST_LITERAL);
	PARSER_TEST_FREE()
}