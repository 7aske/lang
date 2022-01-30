#include "lexer.h"

int main(void){

	Lexer_Result lexer_result;
	Lexer lexer;

	char* code1 = "text == 1;";
	lexer_new(&lexer, code1);
	lexer_lex(&lexer,  &lexer_result);
	assert(lexer_result.size == 4);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_EQ);
	assert(lexer_result.data[2].token == TOK_LIT_INT);
	assert(lexer_result.data[3].token == TOK_SCOL);
	free(lexer_result.data);

	char* code2 = "a: int = 1;";
	lexer_new(&lexer, code2);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_INT);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code3 = "if true { print(1); }";
	lexer_new(&lexer, code3);
	lexer_lex(&lexer, &lexer_result);
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
	lexer_new(&lexer, code4);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_FLT);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code5 = "str: string = \"This is a quoted 'string'\";";
	lexer_new(&lexer, code5);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_STR);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code6 = "str2: string = `This is a quoted string as well`;";
	lexer_new(&lexer, code6);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_STR);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code7 = "ternary?operator:null;";
	lexer_new(&lexer, code7);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_QM);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_COL);
	assert(lexer_result.data[4].token == TOK_NULL);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code8 = "foreach (test: int in tests);";
	lexer_new(&lexer, code8);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 9);
	assert(lexer_result.data[0].token == TOK_FOREACH);
	assert(lexer_result.data[1].token == TOK_LPAREN);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_COL);
	assert(lexer_result.data[4].token == TOK_IDEN);
	assert(lexer_result.data[5].token == TOK_IN);
	assert(lexer_result.data[6].token == TOK_IDEN);
	assert(lexer_result.data[7].token == TOK_RPAREN);
	assert(lexer_result.data[8].token == TOK_SCOL);
	free(lexer_result.data);


	char* code9 = "str: char = 'c';";
	lexer_new(&lexer, code9);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].token == TOK_IDEN);
	assert(lexer_result.data[1].token == TOK_COL);
	assert(lexer_result.data[2].token == TOK_IDEN);
	assert(lexer_result.data[3].token == TOK_ASSN);
	assert(lexer_result.data[4].token == TOK_LIT_CHR);
	assert(lexer_result.data[5].token == TOK_SCOL);
	free(lexer_result.data);

	char* code10 = "str # char = 'c';";
	lexer_new(&lexer, code10);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer.error.size == 1);
	free(lexer_result.data);

	char* code11 = "fn write(file: File, count: u64, byte: u32) -> void {}";
	lexer_new(&lexer, code11);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 19);
	for(u32 i = 0; i < lexer_result.size; ++i) {
		const char* name = token_value[lexer_result.data[i].token];
		printf("%s\n", name == NULL ? "(null)" : name);
	}
	free(lexer_result.data);
}