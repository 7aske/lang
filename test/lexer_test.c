#include "lexer.h"

int main(void) {

	Lexer lexer;

	char* code1 = "text == 1;";
	lexer_new(&lexer, code1);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 4);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_EQ);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_LIT_INT);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code2 = "a: int = 1;";
	lexer_new(&lexer, code2);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_ASSN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LIT_INT);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code3 = "if true { print(1); }";
	lexer_new(&lexer, code3);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 9);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IF);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_TRUE);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_LBRACE);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LPAREN);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_LIT_INT);
	assert(list_get_as(&lexer.tokens, 6, Token*)->type == TOK_RPAREN);
	assert(list_get_as(&lexer.tokens, 7, Token*)->type == TOK_SCOL);
	assert(list_get_as(&lexer.tokens, 8, Token*)->type == TOK_RBRACE);
	lexer_free(&lexer);

	char* code4 = "some_number: float = 1.2141;";
	lexer_new(&lexer, code4);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_ASSN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LIT_FLT);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code5 = "str: string = \"This is a quoted 'string'\";";
	lexer_new(&lexer, code5);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_ASSN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LIT_STR);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code6 = "str2: string = `This is a quoted string as well`;";
	lexer_new(&lexer, code6);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_ASSN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LIT_STR);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code7 = "ternary?operator:null;";
	lexer_new(&lexer, code7);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_QM);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_NULL);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code8 = "foreach (test: int in tests);";
	lexer_new(&lexer, code8);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 9);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_FOREACH);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_LPAREN);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_IN);
	assert(list_get_as(&lexer.tokens, 6, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 7, Token*)->type == TOK_RPAREN);
	assert(list_get_as(&lexer.tokens, 8, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);


	char* code9 = "str: char = 'c';";
	lexer_new(&lexer, code9);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 6);
	assert(list_get_as(&lexer.tokens, 0, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 1, Token*)->type == TOK_COL);
	assert(list_get_as(&lexer.tokens, 2, Token*)->type == TOK_IDEN);
	assert(list_get_as(&lexer.tokens, 3, Token*)->type == TOK_ASSN);
	assert(list_get_as(&lexer.tokens, 4, Token*)->type == TOK_LIT_CHR);
	assert(list_get_as(&lexer.tokens, 5, Token*)->type == TOK_SCOL);
	lexer_free(&lexer);

	char* code10 = "str # char = 'c';";
	lexer_new(&lexer, code10);
	lexer_lex(&lexer);
	assert(lexer.errors.count == 1);
	lexer_free(&lexer);

	char* code11 = "fn write(file: File, count: u64, byte: u32) -> u32 { return 0; }";
	printf("%s\n", code11);
	lexer_new(&lexer, code11);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 22);
	list_foreach(&lexer.tokens, Token*, {
		Token token = *it;
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
	})
	lexer_free(&lexer);


	char* code12 = "true; false; 1.0; 10; 'a'";
	lexer_new(&lexer, code12);
	lexer_lex(&lexer);
	assert(lexer.tokens.count == 9);
	assert(list_get_as(&lexer.tokens, 0, Token*)->integer_value == 1);
	assert(list_get_as(&lexer.tokens, 2, Token*)->integer_value == 0);
	assert(list_get_as(&lexer.tokens, 4, Token*)->float64_value == 1.0);
	assert(list_get_as(&lexer.tokens, 6, Token*)->integer_value == 10);
	assert(strcmp(list_get_as(&lexer.tokens, 8, Token*)->string_value.data, "a")==0);
	lexer_free(&lexer);
}