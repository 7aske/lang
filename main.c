#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "src/token.h"
#include "src/lexer.h"


int main(int argc, char** argv) {
	char* filename = argv[1];
	char* code_text = NULL;

	FILE* fileptr = fopen(filename, "r");
	if (!fileptr) {
		perror("Unable to open file");
		exit(EXIT_FAILURE);
	}

	code_text = (char*) malloc(BUFSIZ);
	if (!code_text) {
		perror("Allocation error");
		exit(EXIT_FAILURE);
	}

	unsigned long size = 0;
	unsigned long n;
	while ((n = fread(code_text + size, BUFSIZ, sizeof(char), fileptr)) > 0) {
		size += n;
		if (n >= BUFSIZ) {
			code_text = realloc(code_text, size);
		}
		if (!code_text) {
			perror("Allocation error");
			exit(EXIT_FAILURE);
		}
	}

	Lexer_Result parsed;
	lexer_lex(code_text, &parsed);

	for (int i = 0; i < parsed.size; ++i) {
		Parsed_Token tok = parsed.data[i];
		char* repr = (char*) token_repr[tok.token];
		char* text = (char*) tok.code_text;
		char* data = (char*) token_value[tok.token];
		printf("%-12s(%d %d) = %s \n",
			   repr == NULL ? "null" : repr,
			   tok.col, tok.row,
			   data == NULL ? text : data);
	}

	LEXER_RESULT_DATA_FREE(&parsed)

	printf("\n%s\n", code_text);

	free(code_text);
}


