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

	Lexer lexer;
	lexer_new(&lexer, code_text);
	lexer_lex(&lexer);
	List parsed = lexer.tokens;

	for (int i = 0; i < parsed.count; ++i) {
		Token tok = *(Token*)(list_get(&parsed, i));
		char* repr = (char*) token_repr[tok.type];
		char* text = (char*) tok.string_value.data;
		char* data = (char*) token_value[tok.type];
		printf("%-12s(%ld %ld - %ld %ld) = '%s' \n",
			   repr == NULL ? "null" : repr,
			   tok.c0, tok.r0,
			   tok.c1, tok.r1,
			   data == NULL ? text : data);
	}

	printf("\n%s\n", code_text);

	free(code_text);
}


