//
// Created by nik on 1/25/22.
//
#include <string.h>
#include "token.h"


Token resolve_token(char* buf, size_t size) {
	size_t len = strnlen(buf, size);
	for (int i = 0; i < __TOK_KEYWORD_END; ++i) {
		const char* tok = token_value[i];
		size_t token_len = strlen(tok);
		if (strncmp(tok, buf, token_len) == 0 && len == token_len) {
			return i;
		}
	}
	return TOK_INVALID;
}

