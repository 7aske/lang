//
// Created by nik on 1/31/22.
//

#ifndef LANG_STACK_H
#define LANG_STACK_H

#include "stdtypes.h"
#include <stdlib.h>
#include <string.h>

typedef struct stack  {
	u64 capacity; // Max capacity
	u64 count;    // Number of elements
	u64 size;     // Size of the individual element
	void* data;   // Stack data
} Stack;


void  stack_new(Stack*, u64);

void  stack_push(Stack*, void*);

bool  stack_pop(Stack*, void* dest);

void* stack_peek(Stack*);

bool  stack_is_empty(Stack*);

void  stack_realloc(Stack*);

#endif //LANG_STACK_H
