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
	return isdigit(*ptr)
		   ||
		   (*ptr == '.' && (PEEK_PREV(ptr) != '.' && PEEK_NEXT(ptr) != '.'));
}

// Checks if the string is a valid number character
// @Incomplete maybe handle scientific notation.
inline bool lexer_is_number(const char* ptr) {
	return isdigit(*ptr) || (*ptr == '.' && PEEK_NEXT(ptr) != '.');
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

// Checks if while parsing the identifier next character is an identifier.
inline bool lexer_startof_linecomment(const char* ptr) {
	return *ptr == '/' && *(ptr + 1) == '/';
}

u32 lexer_eat_iden(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	while (lexer_is_iden(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
	}
	(*code) += string_buffer->count;

	return string_buffer->count;
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
	(*code) += string_buffer->count + placeholder_offset;

	return string_buffer->count;
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

	(*code) += string_buffer->count + quote_count;

	return string_buffer->count + quote_count;

}

void lexer_token_new(Token* dest, Token_Type token, u32 code_size, u32 col, u32 row) {
	// Reset the token data, so we don't copy invalid bytes around
	memset(dest, 0, sizeof(Token));
	dest->type = token;
	// @Incomplete
	if (token == TOK_LIT_STR || token == TOK_IDEN || token == TOK_LIT_CHR) {
		dest->string_value.size = code_size;
		dest->string_value.data = (char*) calloc(code_size + 1, sizeof(char));
	} else if (token == TOK_LIT_FLT) {
		dest->float64_value = 0;
	} else if (token == TOK_LIT_INT || token == TOK_FALSE) {
		dest->integer_value = 0;
	} else if (token == TOK_TRUE) {
		dest->integer_value = 1;
	}

	dest->c0 = col + 1;
	dest->r0 = row + 1;
	dest->c1 = dest->c0 + code_size;
	// @Incomplete will always show the same row as r0
	dest->r1 = row + 1;
}

u32 lexer_lex(Lexer* lexer) {
	char* ptr = lexer->code.text;
	String_Buffer* string_buffer = string_buffer_new();
	u32 size;

	u32 col = 0;
	u32 row = 0;

	// All created tokens are being written to this variable and then
	// copied to the lexer tokens list.
	Token token;

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

		if (lexer_startof_linecomment(ptr)) {
			ptr+= 2; // skip comment start
			while (ptr && *ptr != '\n') {
				col++;
				ptr++;
			}
			// We don't want to skip the ending newline to prevent messing up
			// the row count of the file.
			continue;
		}

		if (lexer_startof_iden(ptr)) {
			size = lexer_eat_iden(&ptr, string_buffer);
			Token_Type possible_token = resolve_token(string_buffer->data,
													  size);
			// If the result is TOK_INVALID that means that the parsed word is
			// an identifier.
			if (possible_token == TOK_INVALID) {
				lexer_token_new(&token, TOK_IDEN, size, col, row);

				// @Optimization this can probably be done at a later step
				// i = 1 because the default type is none
				// for (int i = 1; i < PRIMITIVE_TYPES_LEN; ++i) {
				// 	Primitive_Type type = primitive_types[i];
				// 	if (strcmp(string_buffer->data, type.iden) == 0) {
				// 		token.p_type = type.type;
				// 	}
				// }

				strncpy(token.string_value.data, string_buffer->data,
						string_buffer->count);
			} else {
				lexer_token_new(&token, possible_token, size, col, row);
			}
			list_push(&lexer->tokens, &token);
			col += (int) size;
		} else if (lexer_startof_char(ptr)) {
			size = lexer_eat_char(&ptr, string_buffer);

			if (size == LEXER_FAILED) {
				lexer_report_error(lexer, TOK_LIT_CHR, col, row,
								   "Invalid char literal");
				break;
			}

			lexer_token_new(&token, TOK_LIT_CHR, size, col, row);
			strncpy(token.string_value.data, string_buffer->data,
					string_buffer->count + 1);
			list_push(&lexer->tokens, &token);
			col += 2 + size; // 2 is for two quotes;
		} else if (lexer_startof_string(ptr)) {
			size = lexer_eat_string(&ptr, string_buffer);

			if (size == LEXER_FAILED) {
				lexer_report_error(lexer, TOK_LIT_STR, col, row,
								   "Unquoted string");
				break;
			}

			lexer_token_new(&token, TOK_LIT_STR, string_buffer->count, col,
							row);
			strncpy(token.string_value.data, string_buffer->data,
					string_buffer->count + 1);
			list_push(&lexer->tokens, &token);
			col += (int) size;
		} else if (lexer_startof_number(ptr) && PEEK_NEXT(ptr) != '>') {
			size = lexer_eat_number(&ptr, string_buffer);
			Token_Type token_type = TOK_LIT_INT;
			// @Incomplete validate float not having multiple decimal points
			if (lexer_is_float(string_buffer->data)) {
				token_type = TOK_LIT_FLT;
			}

			lexer_token_new(&token, token_type, size, col, row);
			if (token_type == TOK_LIT_INT) {
				s64 val = strtoll(string_buffer->data, NULL, 10);
				token.integer_value = val;
			} else {
				float64 val = strtod(string_buffer->data, NULL);
				token.float64_value = val;
			}
			list_push(&lexer->tokens, &token);
			col += (int) size;
		} else {
			Token_Type token_type = resolve_operator(ptr);
			if (token_type == TOK_INVALID) {
				lexer_report_error(lexer, token_type, col, row,
								   "Invalid token");
				break;
			}
			// Increment the pointer by the amount of characters 'eaten' by the
			// resolve_operator function.
			// @Optimization not the best way to do this.
			const char* tok_val = token_value[token_type];
			u32 len = strlen(tok_val);
			lexer_token_new(&token, token_type, len, col, row);
			list_push(&lexer->tokens, &token);

			ptr += len;
			col += len;
		}
	}

	string_buffer_free(string_buffer);

	// @Temporary
	list_foreach(&lexer->errors, Lexer_Error_Report*, {
		lexer_print_source_code_location(lexer, it);
		fprintf(stderr, "%s @ %s:%lu:%lu\n", it->text, "__FILE__", it->row,
				it->col);
	})

	return lexer->errors.count;
}


bool lexer_is_float(const char* num_str) {
	return strchr(num_str, '.') != NULL;
}

inline void lexer_new(Lexer* lexer, char* code) {
	lexer->code.size = strlen(code);
	lexer->code.text = code;
	list_new(&lexer->errors, sizeof(Lexer_Error_Report));
	list_new(&lexer->tokens, sizeof(Token));
}

inline void lexer_report_error(Lexer* lexer, Token_Type token_type, u32 col, u32 row, char* error_text) {
	Lexer_Error_Report error_report;
	error_report.text = error_text;
	error_report.type = token_type;
	error_report.col = col + 1;
	error_report.row = row + 1;
	list_push(&lexer->errors, &error_report);
}

void lexer_free(Lexer* lexer) {
	list_free(&lexer->tokens);
	list_free(&lexer->errors);
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
