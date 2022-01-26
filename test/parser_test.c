#include "parser.h"

int main(void){

	Lexer_Result lexer_result;

	char* code3 = "if true { print(1); }";
	lexer_lex(code3, &lexer_result);

	parser_parse(&lexer_result);

	free(lexer_result.data);
}