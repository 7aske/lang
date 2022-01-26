//
// Created by nik on 1/25/22.
//

#include "lexer.h"

// Checks if the string is starting with a valid identifier token.
inline int lexer_startof_iden(const char* ptr) {
	return isalpha(*ptr) || *ptr == '$' || *ptr == '_';
}

// Checks if the string starts with a valid number literal character.
// @Incomplete maybe handle scientific notation.
inline int lexer_startof_number(const char* ptr) {
	return isdigit(*ptr) || *ptr == '-' || *ptr == '.' || *ptr == '+';
}

// Checks if the string is a valid number character
// @Incomplete maybe handle scientific notation.
inline int lexer_is_number(const char* ptr) {
	return isdigit(*ptr) || *ptr == '.';
}

// Checks if the string starts with a valid quote for identifying a string.
inline int lexer_startof_string(const char* ptr) {
	return *ptr == '\'' || *ptr == '"' || *ptr == '`';
}

// Checks if while parsing the identifier next character is an identifier.
inline int lexer_is_iden(const char* ptr) {
	return (isalnum(*ptr) || *ptr == '$' || *ptr == '_') && !isspace(*ptr);
}

size_t lexer_eat_iden(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	while (lexer_is_iden(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
	}
	(*code) += string_buffer->size;

	return string_buffer->size;
}

Token lexer_eat_token(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	Token retval = TOK_INVALID;
	while (!isspace(*ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
		Token token = resolve_token(string_buffer->data, string_buffer->size);
		if (token != TOK_INVALID) {
			retval = token;
			break;
		}
	}
	(*code) += string_buffer->size;

	return retval;
}


size_t lexer_eat_number(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	// First iteration has a different condition than the rest of the loop
	if (lexer_startof_number(ptr)) {
		string_buffer_append_char(string_buffer, *ptr++);
	}

	size_t placeholder_offset = 0;

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

Lexer_Error lexer_eat_string(char** code, String_Buffer* string_buffer) {
	char* ptr = *code;
	char quote = *ptr++;
	int quote_count = 1;
	char c;
	while (not_terminated(ptr)) {
		c = *ptr;
		// If it's the unescaped initial type of quote
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

void lexer_token_new(Parsed_Token* dest, Token token, size_t code_size, int col, int row) {
	dest->token = token;
	dest->code_size = code_size;
	dest->code_text = (char*) calloc(code_size, sizeof(char));
	dest->col = col;
	dest->row = row;
}

int lexer_lex(char* buffer, Lexer_Result* data) {
	char* ptr = buffer;
	String_Buffer* string_buffer = string_buffer_new();
	size_t size;

	size_t data_size = 32;
	data->data = (Parsed_Token*) calloc(data_size, sizeof(Parsed_Token));
	data->size = 0;

	int col = 0;
	int row = 0;
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
			Token possible_token = resolve_token(string_buffer->data, size);
			if (possible_token == TOK_INVALID) {
				lexer_token_new(&data->data[data->size], TOK_IDEN, size, col, row);
				strncpy(data->data[data->size].code_text, string_buffer->data, string_buffer->size);
			} else {
				lexer_token_new(&data->data[data->size], possible_token, 0, col, row);
			}
			data->size++;
			col += (int) size;
			continue;
		}

		if (lexer_startof_string(ptr)) {
			size = lexer_eat_string(&ptr, string_buffer);

			if (size == LEXER_FAILED) {
				report_lexer_error("Unquoted string")
				break;
			}

			lexer_token_new(&data->data[data->size], TOK_LIT_STR, size, col, row);
			strncpy(data->data[data->size].code_text, string_buffer->data, string_buffer->size + 1);
			data->size++;
			col += (int) size;
			continue;
		}

		if (lexer_startof_number(ptr)) {
			size = lexer_eat_number(&ptr, string_buffer);
			Token token = TOK_LIT_INT;
			// @Incomplete validate float not having multiple decimal points
			if (lexer_is_float(string_buffer->data)) {
				token = TOK_LIT_FLT;
			}

			lexer_token_new(&data->data[data->size], token, size, col, row);
			strncpy(data->data[data->size].code_text, string_buffer->data, string_buffer->size);
			data->size++;
			col += (int) size;
			continue;
		}

		Token token = resolve_operator(ptr);
		if (token == TOK_INVALID) {
			report_lexer_error("Invalid token")
			break;
		}
		// Increment the pointer by the amount of characters 'eaten' by the
		// resolve_operator function.
		// @Optimization not the best way to do this.
		const char* tok_val = token_value[token];
		ptr += strlen(tok_val);

		lexer_token_new(&data->data[data->size], token, 0, col, row);
		data->size++;

		if (data->size == data_size) {
			data_size *= 2;
			data->data = realloc(data->data, data_size * sizeof(Parsed_Token));
		}

	}

	string_buffer_free(string_buffer);

	// @Temporary
	lexer_error_foreach({
	   fprintf(stderr, "%s\n", it);
	})

	return LEXER_ERROR_COUNT;
}

int lexer_is_float(const char* num_str) {
	return strchr(num_str, '.') != NULL;
}
