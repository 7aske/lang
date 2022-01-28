#include "parser.h"

int main(void){

	Lexer_Result lexer_result;

	char* code3 = "if true 1 else 2;";
	lexer_lex(code3, &lexer_result);

	Parser_Result result = parser_parse(&lexer_result);

	Ast_Node* first_node = result.nodes[0];

	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_LITERAL);
	assert(first_node->left->type == AST_LITERAL);
	assert(first_node->right->type == AST_LITERAL);

	free(lexer_result.data);
}