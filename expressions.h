/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file expressions.c Hlavickovy subor precedencnej analyzy
 * @author Štefan Martiček
 */
 
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "types.h"
#include "scanner.h"

/* Uplne funkcne prototypy funkcii syntax.c */
E_ERROR_TYPE precedenceInit ( void );
E_ERROR_TYPE evaluate_expr ( T_token * token, TOKEN_TYPE termination_ttype );
void precedenceShutDown ( void );

#endif //EXPRESSIONS_H

