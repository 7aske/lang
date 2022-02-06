#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "src/token.h"
#include "src/lexer.h"
#include "src/parser.h"
#include "src/interpreter.h"


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
	Parser parser;
	lexer_new(&lexer, code_text);
	lexer_lex(&lexer);
	parser_new(&parser, code_text);
	Parser_Result result = parser_parse(&parser, &lexer);

	Interpreter interpreter;
	interpreter_new(&interpreter, &result.nodes);
	interpreter_run(&interpreter);

	stack_foreach(&interpreter.instructions, Instruction*, {
		printf("%d %llu %llu\n", it->bytecode, it->args[0], it->args[1]);
	})

	free(code_text);
}


