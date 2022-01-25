#include "lexer.h"

int main(void){

	Lexer_Result lexer_result;

	char* code1 = "text == 1;";
	lexer_lex(code1, &lexer_result);
	assert(lexer_result.size == 4);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_EQ);
	assert(lexer_result.data[2].token == TOK_LIT_INT);
	assert(lexer_result.data[3].token == TOK_SCOL);
	free(lexer_result.data);

	char* code2 = "a: int = 1;";
	lexer_lex(code2, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_INT);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code3 = "if true { print(1); }";
	lexer_lex(code3, &lexer_result);
	assert(lexer_result.size == 9);
	assert(lexer_result.data[0].token == TOK_IF);
	assert(lexer_result.data[1].token == TOK_TRUE);
	assert(lexer_result.data[2].token == TOK_LBRACE);
	assert(lexer_result.data[3].token == TOK_IDEN);
	assert(lexer_result.data[4].token == TOK_LPAREN);
	assert(lexer_result.data[5].token == TOK_LIT_INT);
	assert(lexer_result.data[6].token == TOK_RPAREN);
	assert(lexer_result.data[7].token == TOK_SCOL);
	assert(lexer_result.data[8].token == TOK_RBRACE);
	free(lexer_result.data);

	char* code4 = "some_number: float = 1.2141;";
	lexer_lex(code4, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_FLT);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code5 = "str: string = \"This is a quoted 'string'\";";
	lexer_lex(code5, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_STR);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code6 = "str2: string = `This is a quoted string as well`;";
	lexer_lex(code6, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_STR);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code7 = "ternary?operator:null;";
	lexer_lex(code7, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_QM);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_COL);
	assert(lexer_result.data[4].token == TOK_NULL);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);
}