//
// Created by nik on 1/25/22.
//
#include "token.h"


Token_Type resolve_token(char* buf, u32 size) {
	u32 len = strnlen(buf, size);
	for (int i = 1; i < __TOK_KEYWORD_END; ++i) {
		const char* tok = token_value[i];
		u32 token_len = strlen(tok);
		if (strncmp(tok, buf, token_len) == 0 && len == token_len) {
			return i;
		}
	}
	return TOK_INVALID;
}

Token_Type resolve_operator(const char* ptr) {
	if (*ptr == '!') {
		if (*(ptr + 1) == '=') {
			return TOK_NE;
		} else {
			return TOK_NOT;
		}
	}

	if (*ptr == '%') { return TOK_MOD; }
	if (*ptr == '&') {
		if (*(ptr + 1) == '&') {
			return TOK_AND;
		} else {
			return TOK_INVALID;
		}
	}

	if (*ptr == '(') {
		return TOK_LPAREN;
	}
	if (*ptr == ')') {
		return TOK_RPAREN;
	}
	if (*ptr == '*') {
		return TOK_MUL;
	}
	if (*ptr == '+') {
		if (*(ptr + 1) == '+') {
			return TOK_INC;
		} else {
			return TOK_ADD;
		}
	}
	if (*ptr == ',') {
		return TOK_COMMA;
	}
	if (*ptr == '-') {
		if (*(ptr + 1) == '-') {
			return TOK_DEC;
		} else if (*(ptr + 1) == '>') {
			return TOK_THIN_ARRW;
		} else {
			return TOK_SUB;
		}
	}
	if (*ptr == '.') {
		if (*(ptr + 1) == '.') {
			return TOK_DDOT;
		} else {
			return TOK_DOT;
		}
	}
	if (*ptr == '/') {
		return TOK_DIV;
	}
	if (*ptr == ':') {
		return TOK_COL;
	}
	if (*ptr == ';') {
		return TOK_SCOL;
	}
	if (*ptr == '<') {
		if (*(ptr + 1) == '=') {
			return TOK_LE;
		} else {
			return TOK_LT;
		}
	}
	if (*ptr == '=') {
		if (*(ptr + 1) == '=') {
			return TOK_EQ;
		} else if (*(ptr + 1) == '>') {
			return TOK_FAT_ARRW;
		} else {
			return TOK_ASSN;
		}
	}
	if (*ptr == '>') {
		if (*(ptr + 1) == '=') {
			return TOK_GE;
		} else {
			return TOK_GT;
		}
	}

	if (*ptr == '?') {
		if (*(ptr + 1) == '?') {
			return TOK_DQM;
		} else {
			return TOK_QM;
		}
	}

	if (*ptr == '[') {
		return TOK_LBRACK;
	}
	if (*ptr == ']') {
		return TOK_RBRACK;
	}
	if (*ptr == '{') {
		return TOK_LBRACE;
	}
	if (*ptr == '|') {
		if (*(ptr + 1) == '|') {
			return TOK_OR;
		} else {
			return TOK_INVALID;
		}
	}
	if (*ptr == '}') {
		return TOK_RBRACE;
	}

	return TOK_INVALID;
}
