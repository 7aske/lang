#include "parser.h"

int main(void){

	Lexer_Result lexer_result;
	Parser_Result result;
	Ast_Node* first_node = NULL;

	char* code1 = "if true {1} else {2};";
	lexer_lex(code1, &lexer_result);
	result = parser_parse(&lexer_result);

	first_node = result.nodes[0];
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_LITERAL);
	assert(first_node->left->type == AST_BLOCK);
	assert(first_node->left->nodes[0]->type == AST_LITERAL);
	assert(first_node->right->type == AST_BLOCK);
	assert(first_node->right->nodes[0]->type == AST_LITERAL);
	free(lexer_result.data);

	char* code2 = "if a == true {2} else {3}; } ";
	lexer_lex(code2, &lexer_result);
	result = parser_parse(&lexer_result);

	first_node = result.nodes[0];
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_EQUALITY);
	assert(first_node->middle->left->type == AST_IDENT);
	assert(first_node->middle->right->type == AST_LITERAL);
	assert(first_node->left->type == AST_BLOCK);
	assert(first_node->left->nodes[0]->type == AST_LITERAL);
	assert(first_node->right->type == AST_BLOCK);
	assert(first_node->right->nodes[0]->type == AST_LITERAL);
	free(lexer_result.data);
}