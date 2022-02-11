//
// Created by nik on 2/10/22.
//

#ifndef LANG_UTIL_H
#define LANG_UTIL_H

#include <string.h>

#define PAD_TO(__end, __str) for (int _i = 0; _i < (__end); _i++) {\
fputs(__str, stderr);\
}

#define REPORT_LINE(__Str, ...) \
fprintf(stderr, __Str" %s:%d:1\n", ##__VA_ARGS__, __FILE__, __LINE__)

// @formatter:off
enum term_color {
	TEXT_BLACK   = 30,
	TEXT_RED     = 31,
	TEXT_GREEN   = 32,
	TEXT_YELLOW  = 33,
	TEXT_BLUE    = 34,
	TEXT_MAGENTA = 35,
	TEXT_CYAN    = 36,
	TEXT_WHITE   = 37,
	TEXT_DEFAULT = 39,
	BG_BLACK     = 40,
	BG_RED       = 41,
	BG_GREEN     = 42,
	BG_YELLOW    = 43,
	BG_BLUE      = 44,
	BG_MAGENTA   = 45,
	BG_CYAN      = 46,
	BG_WHITE     = 47,
	BG_DEFAULT   = 49,
};
// @formatter:on

#define COLOR(__color) fprintf(stderr, "\033[%dm", __color)
#define CLEAR COLOR(0)


static void print_source_code_location(const char* code, u32 start_col, u64 start_row, u32 end_col) {
	const char* print_start = NULL;

	while (1) {

		if (*code == '\0')
			start_row--;

		if (*code == '\n')
			start_row--;

		if (start_row == 1)
			break;

		code++;
	}

	print_start = code;

	if (*print_start == '\n')
		print_start++;

	COLOR(TEXT_RED);
	while (*print_start != '\n') {
		if (*print_start == '\0') {
			fputc('\n', stderr);
			break;
		} else {
			if (*print_start == '\t'){
				fputc(' ', stderr);
			} else {
				fputc(*print_start, stderr);
			}
			print_start++;
		}
	}
	CLEAR;

	if (*print_start == '\n')
		fputc('\n', stderr);

	// COLOR(TEXT_RED);
	PAD_TO(start_col - 1, " ")
	PAD_TO(end_col - start_col, "^")
	// CLEAR;
	fputc('\n', stderr);
	PAD_TO(start_col - 1, "─")
	fputs("┘", stderr);
	fputc('\n', stderr);
}

static void print_token_source_code_location(const char* code, Token* token) {
	print_source_code_location(code, token->c0, token->r0, token->c1);
}

#endif //LANG_UTIL_H
