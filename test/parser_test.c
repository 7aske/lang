#include "parser.h"

int main(void){

	Lexer_Result lexer_result;
	Parser_Result result;
	Ast_Node* first_node = NULL;
	Parser parser;
	Lexer lexer;

	char* code1 = "if true {1} else {2};";
	lexer_new(&lexer, code1);
	lexer_lex(&lexer, &lexer_result);
	parser_new(&parser, code1);
	result = parser_parse(&parser, &lexer_result);

	first_node = result.nodes[0];
	assert(first_node->type == AST_IF);
	assert(first_node->middle->type == AST_LITERAL);
	assert(first_node->left->type == AST_BLOCK);
	assert(first_node->left->nodes[0]->type == AST_LITERAL);
	assert(first_node->right->type == AST_BLOCK);
	assert(first_node->right->nodes[0]->type == AST_LITERAL);
	free(lexer_result.data);
	parser_free(&parser);

	char* code2 = "if a == true {2} else {3};";
	lexer_new(&lexer, code2);
	lexer_lex(&lexer, &lexer_result);
	parser_new(&parser, code2);
	result = parser_parse(&parser, &lexer_result);

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
	parser_free(&parser);

	char* code3 = "a == true { function(); }}";
	lexer_new(&lexer, code3);
	lexer_lex(&lexer, &lexer_result);
	parser_new(&parser, code3);
	result = parser_parse(&parser, &lexer_result);
	assert(result.error_count == 1);
	free(lexer_result.data);
	parser_free(&parser);
}