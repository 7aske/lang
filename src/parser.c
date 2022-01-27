//
// Created by nik on 1/26/22.
//

#include "parser.h"

Ast_Node* parse_statement(Token * lexer_result){

}

void parser_parse(Lexer_Result* lexer_result) {
	Token* parsed_tokens = lexer_result->data;
	Token* end = lexer_result->data + lexer_result->size;

	while(parsed_tokens != end) {

		parsed_tokens++;
	}
}
