//
// Created by nik on 1/26/22.
//

#include "parser.h"

void parser_parse(Lexer_Result* lexer_result) {
	Parsed_Token* parsed_tokens = lexer_result->data;
	Parsed_Token* end = lexer_result->data + lexer_result->size;

	while(parsed_tokens != end) {

		parsed_tokens++;
	}
}
