//
// Created by nik on 2/6/22.
//

#ifndef LANG_BYTECODE_H
#define LANG_BYTECODE_H

#include "stdtypes.h"

typedef enum bytecode {
	BC_INVALID = 0,
	BC_ADD,
	BC_LOAD
} Bytecode;

typedef struct instruction {
	Bytecode bytecode;
	u64      args[4];
} Instruction;

typedef enum decode_result_type {
	DRT_VALUE,
	DRT_INSTRUCTION,
	DRT_ADDRESS,
} Decode_Result_Type;

typedef struct decode_result {
	Decode_Result_Type type;
	Instruction instruction;
	u64 value;
} Decode_Result;

#endif //LANG_BYTECODE_H
