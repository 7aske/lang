#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "src/lexer.h"
#include "src/parser.h"
#include "src/interpreter.h"

#ifdef _WIN32
#include <windows.h>
#endif

long long get_file_size(FILE* file) {
	struct stat stat_info;

	if (fstat(fileno(file), &stat_info) == 0) {
		return stat_info.st_size;
	}

	return -1;
}

int main(int argc, char** argv) {
	char* input_filename;
	List input_libraries;
	list_new(&input_libraries, sizeof(char*));
	char* code_text = NULL;
	char* output_filename = "a.out";
	char* compiler_command = "cc";

	char* argptr;
	int option;
	while ((option = getopt(argc, argv, ":o:C:l:")) != -1) {
		switch (option) {
			case 'o':
				output_filename = strdup(optarg);
				break;
			case 'l':
				argptr = strdup(optarg);
				list_push(&input_libraries, &argptr);
				break;
			case 'C':
				compiler_command = strdup(optarg);
				break;
			default:
				printf("unknown option: -%c\n", optopt);
				exit(EXIT_FAILURE);
		}
	}

	if (optind != argc - 1) {
		errno = EINVAL;
		perror("No files provided");
		exit(EXIT_FAILURE);
	}

	// Fix non-ascii characters printing on Windows
	#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	#endif

	input_filename = strdup(argv[optind]);

	FILE* fileptr = fopen(input_filename, "r");
	if (!fileptr) {
		COLOR(TEXT_RED);
		perror("Unable to open file");
		CLEAR;
		exit(EXIT_FAILURE);
	}

	long long file_size = get_file_size(fileptr);
	if (file_size == -1) {
		COLOR(TEXT_RED);
		perror("Unable to get file size");
		CLEAR;
		exit(EXIT_FAILURE);
	}

	code_text = (char*) malloc(file_size + 1);
	if (!code_text) {
		perror("Allocation error");
		exit(EXIT_FAILURE);
	}

	char* ptr = code_text;
	int c;
	while ((c = fgetc(fileptr)) != EOF) {
		*ptr++ = (char) c;
	}
	*ptr = '\0';


	Lexer lexer;
	Parser parser;
	lexer_new(&lexer, code_text);

	// If there are errors report them before parsing and exit.
	if (lexer_lex(&lexer)) {
		list_foreach(&lexer.errors, Lexer_Error_Report*, {
			fprintf(stderr, "%s @ %s:%lu:%lu\n", it->text, input_filename, it->row,
					it->col);
			print_source_code_location(lexer.code.text, it->col, it->row, it->col + 1);
		})
		return (int) lexer.errors.count;
	}


	if (lexer.errors.count == 0) {
		char* asm_filename = (char*) calloc(strlen(output_filename) + 3, sizeof(char));
		strcpy(asm_filename, output_filename);
		FILE* output = fopen(strcat(asm_filename, ".s"), "w+");
		if (!output) {
			COLOR(TEXT_RED);
			perror("Unable to open file");
			CLEAR;
			exit(EXIT_FAILURE);
		}

		parser_new(&parser, code_text);

		// @Temporary need to add input_filename in parser "constructor"
		parser.code.filename = input_filename;
		// @Temporary
		parser.output = output;

		Parser_Result result = parser_parse(&parser, &lexer);

		if (result.errors.count != 0) {
			fflush(output);
			fclose(output);
			return (s16) result.errors.count;
		}


		Interpreter interpreter;
		interpreter_new(&interpreter, &result.nodes, output);
		// @Temporary need to add these in constructor
		interpreter.code = parser.code.text;
		interpreter.input_filename = input_filename;
		interpreter_run(&interpreter);

		fflush(output);
		fclose(output);

		// @Refactor there is probably a cleaner way to do this.
		char command_buffer[BUFSIZ];
		sprintf(command_buffer, "%s -o %s %s",
				compiler_command,
				output_filename,
				asm_filename);

		list_foreach(&input_libraries, char**, {
			strcat(command_buffer, " ");
			strcat(command_buffer, *it);
		})

		COLOR(TEXT_CYAN);
		fprintf(stderr, "INFO: Linking with command\n");
		CLEAR;
		fprintf(stderr, "%s\n", command_buffer);

		// @Temporary
		system(command_buffer);
	}

	return (s16) lexer.errors.count;
}


