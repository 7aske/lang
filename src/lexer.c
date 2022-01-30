//
// Created by nik on 1/25/22.
//

#include "lexer.h"

// Checks if the string is starting with a valid identifier type.
inline bool lexer_startof_iden(const char* ptr) {
	return isalpha(*ptr) || *ptr == '$' || *ptr == '_';
}

// Checks if the string starts with a valid number literal character.
// @Incomplete maybe handle scientific notation.
inline bool lexer_startof_number(const char* ptr) {
	return isdigit(*ptr) || *ptr == '-' || *ptr == '.' || *ptr == '+';
}

// Checks if the string is a valid number character
// @Incomplete maybe handle scientific notation.
inline bool lexer_is_number(const char* ptr) {
	return isdigit(*ptr) || *ptr == '.';
}

// Checks if the string starts with a valid quote for identifying a string.
inline bool lexer_startof_string(const char* ptr) {
	return *ptr == '\'' || *ptr == '"' || *ptr == '`';
}

// Checks if the character starts with a valid quote for identifying a character literal
bool lexer_startof_char(const char* ptr) {
	return *ptr == '\'';
}

// Checks if while parsing the identifier next character is an identifier.
inline bool lexer_is_iden(const char* ptr) {
	return (isalnum(*ptr) || *ptr == '$' || *ptr == '_') && !isspace(*ptr);
}

u32 lexer_eat_iden(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	while (lexer_is_iden(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
	}
	(*code) += string_buffer->size;

	return string_buffer->size;
}

Token_Type lexer_eat_token(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	Token_Type retval = TOK_INVALID;
	while (!isspace(*ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
		Token_Type token = resolve_token(string_buffer->data,
										 string_buffer->size);
		if (token != TOK_INVALID) {
			retval = token;
			break;
		}
	}
	(*code) += string_buffer->size;

	return retval;
}


u32 lexer_eat_number(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	// First iteration has a different condition than the rest of the loop
	if (lexer_startof_number(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
	}

	u32 placeholder_offset = 0;

	while (lexer_is_number(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
		// if we encounter _ placeholders within the number literal
		// we skip it.
		if (*ptr == '_') {
			placeholder_offset++;
			ptr++;
		}
	}
	(*code) += string_buffer->size + placeholder_offset;

	return string_buffer->size;
}

Lexer_Error lexer_eat_char(char** code, String_Buffer* string_buffer) {
	// @Incomplete handle multi-char char literals?
	// We assume *code is "'"
	(*code)++; // skip '
	string_buffer_append_char(string_buffer, **code);
	if (*(++*code) != '\'') {
		(*code) -= 2; // reset?
		return LEXER_FAILED;
	}
	(*code)++;

	return 1;
}

Lexer_Error lexer_eat_string(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	char quote = *ptr++;
	int quote_count = 1;
	char c;
	while (not_terminated(ptr)) {
		c = *ptr;
		// If it's the unescaped initial error of quote
		if (c == quote && *(ptr - 1) != '\\') {
			quote_count++;
			break;
		} else if (c == quote) {
			quote_count++;
		}

		string_buffer_append_char(string_buffer, c);
		ptr++;
	}

	// @Incomplete handle error for miss matched quotes
	if (quote_count % 2 != 0) {
		return LEXER_FAILED;
	}

	(*code) += string_buffer->size + quote_count;

	return string_buffer->size + quote_count;

}

void lexer_token_new(Token* dest, Token_Type token, u32 code_size, u32 col, u32 row) {
	dest->type = token;
	// @Incomplete
	if (token == TOK_LIT_STR || token == TOK_IDEN || token == TOK_LIT_CHR) {
		dest->string_value.size = code_size;
		dest->string_value.data = (char*) calloc(code_size + 1, sizeof(char));
	} else if (token == TOK_LIT_FLT) {
		dest->float64_value = 0;
	} else if (token == TOK_LIT_INT || token == TOK_TRUE || token == TOK_FALSE) {
		dest->integer_value = 0;
	}
	dest->c0 = col + 1;
	dest->r0 = row + 1;
	dest->c1 = dest->c0 + code_size;
	// @Incomplete will always show the same row as r0
	dest->r1 = row + 1;
}

u32 lexer_lex(Lexer* lexer, Lexer_Result* result) {
	char* ptr = lexer->code.text;
	String_Buffer* string_buffer = string_buffer_new();
	u32 size;

	result->capacity = 16;
	result->size = 0;
	result->data = (Token*) calloc(result->capacity, sizeof(Token));

	u32 col = 0;
	u32 row = 0;
	while (not_terminated(ptr)) {
		if (isspace(*ptr)) {
			if (*ptr == '\n') {
				row++;
				col = 0;
			} else {
				col++;
			}
			ptr++;
			continue;
		}

		// We reset the buffer before using it
		string_buffer_cut(string_buffer);

		if (lexer_startof_iden(ptr)) {
			size = lexer_eat_iden(&ptr, string_buffer);
			Token_Type possible_token = resolve_token(string_buffer->data, size);
			// If the result is TOK_INVALID that means that the parsed word is
			// an identifier.
			if (possible_token == TOK_INVALID) {
				lexer_token_new(&result->data[result->size], TOK_IDEN, size, col, row);
				strncpy(result->data[result->size].string_value.data, string_buffer->data, string_buffer->size);
			} else {
				lexer_token_new(&result->data[result->size], possible_token, size, col, row);
			}
			result->size++;
			col += (int) size;
			goto resize;
		}


		if (lexer_startof_char(ptr)) {
			size = lexer_eat_char(&ptr, string_buffer);

			if (size == LEXER_FAILED) {
				lexer_report_error(lexer, TOK_LIT_CHR, col, row, "Invalid char literal");
				break;
			}
			lexer_token_new(&result->data[result->size], TOK_LIT_CHR, size, col, row);
			strncpy(result->data[result->size].string_value.data, string_buffer->data, string_buffer->size + 1);
			result->size++;
			col += 2 + size; // 2 is for two quotes;
			goto resize;
		}

		if (lexer_startof_string(ptr)) {
			size = lexer_eat_string(&ptr, string_buffer);

			if (size == LEXER_FAILED) {
				lexer_report_error(lexer, TOK_LIT_STR, col, row, "Unquoted string");
				break;
			}

			lexer_token_new(&result->data[result->size], TOK_LIT_STR, string_buffer->size, col, row);
			strncpy(result->data[result->size].string_value.data, string_buffer->data, string_buffer->size + 1);
			result->size++;
			col += (int) size;
			goto resize;
		}

		if (lexer_startof_number(ptr) && PEEK_NEXT(ptr) != '>') {
			size = lexer_eat_number(&ptr, string_buffer);
			Token_Type token = TOK_LIT_INT;
			// @Incomplete validate float not having multiple decimal points
			if (lexer_is_float(string_buffer->data)) {
				token = TOK_LIT_FLT;
			}

			lexer_token_new(&result->data[result->size], token, size, col, row);
			if (token == TOK_LIT_INT) {
				s64 val = strtoll(string_buffer->data, NULL, 10);
				result->data[result->size].integer_value = val;
			} else {
				float64 val = strtod(string_buffer->data, NULL);
				result->data[result->size].float64_value = val;
			}
			result->size++;
			col += (int) size;
			goto resize;
		}

		Token_Type token = resolve_operator(ptr);
		if (token == TOK_INVALID) {
			lexer_report_error(lexer, token, col, row, "Invalid token");
			break;
		}
		// Increment the pointer by the amount of characters 'eaten' by the
		// resolve_operator function.
		// @Optimization not the best way to do this.
		const char* tok_val = token_value[token];
		u32 len = strlen(tok_val);
		lexer_token_new(&result->data[result->size], token, len, col, row);
		result->size++;

		ptr += len;
		col += len;

		resize:
		if (result->size == result->capacity) {
			result->capacity *= 2;
			result->data = reallocarray(result->data, result->capacity,
										sizeof(Token));
		}

	}

	string_buffer_free(string_buffer);

	// @Temporary
	lexer_error_foreach(lexer, {
		lexer_print_source_code_location(lexer, &it);
		fprintf(stderr, "%s @ %s:%lu:%lu\n", it.text, "__FILE__", it.row, it.col);
	})

	return lexer->error.size;
}


bool lexer_is_float(const char* num_str) {
	return strchr(num_str, '.') != NULL;
}

inline void lexer_new(Lexer* lexer, char* code) {
	lexer->code.size = strlen(code);
	lexer->code.text = code;
	lexer->error.capacity = 16;
	lexer->error.size = 0;
	lexer->error.reports =
		(Lexer_Error_Report*) calloc(lexer->error.capacity,
									  sizeof(Lexer_Error_Report));
}

inline void lexer_report_error(Lexer* lexer, Token_Type token_type, u32 col, u32 row, char* error_text) {
	Lexer_Error_Report error_report;
	error_report.text = error_text;
	error_report.type = token_type;
	error_report.col = col + 1;
	error_report.row = row + 1;
	memcpy(lexer->error.reports + lexer->error.size++,
		   &error_report,
		   sizeof(Lexer_Error_Report));
	if (lexer->error.size == lexer->error.capacity) {
		lexer->error.capacity *= 2;
		lexer->error.reports =
			reallocarray(lexer->error.reports,
						 lexer->error.capacity,
						 sizeof(Lexer_Error_Report));
	}
}

void lexer_free(Lexer* lexer) {
	lexer->error.size = 0;
	free(lexer->error.reports);
	lexer->error.reports = NULL;
}

// @CopyPaste
void lexer_print_source_code_location(Lexer* lexer, Lexer_Error_Report* report) {
	u32 start_col = report->col;
	u32 start_row = report->row;
	u32 end_col = report->col + 1;

	char* code = lexer->code.text;
	char* print_start = NULL;

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

	while (*print_start != '\n') {
		if (*print_start == '\0') {
			fputc('\n', stderr);
			break;
		} else {
			fputc(*print_start, stderr);
			print_start++;
		}
	}

	PAD_TO(start_col - 1, " ")
	PAD_TO(end_col - start_col, "^")
	fputc('\n', stderr);
	PAD_TO(start_col - 1, " ")
	fputc('|', stderr);
	fputc('\n', stderr);
	PAD_TO(start_col - 1, "─")
	fputs("┘", stderr);
	fputc('\n', stderr);
}
