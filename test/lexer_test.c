#include "lexer.h"

int main(void) {

	Lexer_Result lexer_result;
	Lexer lexer;

	char* code1 = "text == 1;";
	lexer_new(&lexer, code1);
	lexer_lex(&lexer,  &lexer_result);
	assert(lexer_result.size == 4);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_EQ);
	assert(lexer_result.data[2].type == TOK_LIT_INT);
	assert(lexer_result.data[3].type == TOK_SCOL);
	free(lexer_result.data);

	char* code2 = "a: int = 1;";
	lexer_new(&lexer, code2);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_COL);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_ASSN);
	assert(lexer_result.data[4].type == TOK_LIT_INT);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code3 = "if true { print(1); }";
	lexer_new(&lexer, code3);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 9);
	assert(lexer_result.data[0].type == TOK_IF);
	assert(lexer_result.data[1].type == TOK_TRUE);
	assert(lexer_result.data[2].type == TOK_LBRACE);
	assert(lexer_result.data[3].type == TOK_IDEN);
	assert(lexer_result.data[4].type == TOK_LPAREN);
	assert(lexer_result.data[5].type == TOK_LIT_INT);
	assert(lexer_result.data[6].type == TOK_RPAREN);
	assert(lexer_result.data[7].type == TOK_SCOL);
	assert(lexer_result.data[8].type == TOK_RBRACE);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code4 = "some_number: float = 1.2141;";
	lexer_new(&lexer, code4);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_COL);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_ASSN);
	assert(lexer_result.data[4].type == TOK_LIT_FLT);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code5 = "str: string = \"This is a quoted 'string'\";";
	lexer_new(&lexer, code5);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_COL);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_ASSN);
	assert(lexer_result.data[4].type == TOK_LIT_STR);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code6 = "str2: string = `This is a quoted string as well`;";
	lexer_new(&lexer, code6);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_COL);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_ASSN);
	assert(lexer_result.data[4].type == TOK_LIT_STR);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code7 = "ternary?operator:null;";
	lexer_new(&lexer, code7);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_QM);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_COL);
	assert(lexer_result.data[4].type == TOK_NULL);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code8 = "foreach (test: int in tests);";
	lexer_new(&lexer, code8);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 9);
	assert(lexer_result.data[0].type == TOK_FOREACH);
	assert(lexer_result.data[1].type == TOK_LPAREN);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_COL);
	assert(lexer_result.data[4].type == TOK_IDEN);
	assert(lexer_result.data[5].type == TOK_IN);
	assert(lexer_result.data[6].type == TOK_IDEN);
	assert(lexer_result.data[7].type == TOK_RPAREN);
	assert(lexer_result.data[8].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);


	char* code9 = "str: char = 'c';";
	lexer_new(&lexer, code9);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 6);
	assert(lexer_result.data[0].type == TOK_IDEN);
	assert(lexer_result.data[1].type == TOK_COL);
	assert(lexer_result.data[2].type == TOK_IDEN);
	assert(lexer_result.data[3].type == TOK_ASSN);
	assert(lexer_result.data[4].type == TOK_LIT_CHR);
	assert(lexer_result.data[5].type == TOK_SCOL);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code10 = "str # char = 'c';";
	lexer_new(&lexer, code10);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer.errors.count == 1);
	free(lexer_result.data);
	lexer_free(&lexer);

	char* code11 = "fn write(file: File, count: u64, byte: u32) -> u32 { return 0; }";
	printf("%s\n", code11);
	lexer_new(&lexer, code11);
	lexer_lex(&lexer, &lexer_result);
	assert(lexer_result.size == 22);
	for (u32 i = 0; i < lexer_result.size; ++i) {
		Token token = lexer_result.data[i];
		const char* name = token_repr[token.type];
		const char* value = token_value[token.type];
		if (token.type == TOK_LIT_INT) {
			u64 int_value = token.integer_value;
			printf("%16s %llu\n", name, int_value);
		} else if (token.type == TOK_LIT_FLT) {
			float64 float_value = token.float64_value;
			printf("%16s %.2f\n", name, float_value);
		} else {
			const char* str_value = token.string_value.data;
			printf("%16s %s\n", name, value == NULL ? str_value : value);
		}
	}
	free(lexer_result.data);
	lexer_free(&lexer);

}