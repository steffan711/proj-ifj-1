/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file runtime.h Header file pre interpret
 * @author Oliver Nemček
 */


#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include "generator.h"
#include "scanner.h"
#include "types.h"

/* STACK */

typedef struct context_struct
{
    unsigned int used;
    T_DVAR local[];
} Vector;

typedef struct stack_struct
{
    int actual;
    int size;
    Vector* (bucket[]);
} Stack;

extern T_DVAR retval;

E_ERROR_TYPE InterpretCode( Instruction *EntryPoint );
void RuntimeErrorCleanup(void);

#endif // RUNTIME_H