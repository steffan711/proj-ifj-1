#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include "generator.h"
#include "scanner.h"
#include "types.h"

/* STACK */

typedef struct context_struct
{
    unsigned int size;
    Instruction* EIP;
    T_DVAR local[];
} Context;

typedef struct stack_struct
{
    int top;
    int size;
    bool call_in_progress;
    Context* *array;
} Stack;

extern T_DVAR retval;

E_ERROR_TYPE InterpretCode( Instruction *EntryPoint );

#endif // RUNTIME_H