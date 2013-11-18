/**
 * @file expressions.h
 *
 * @brief Hlavickovy subor pre syntakticky analyzator - vyhodnocovanie vyrazov
 * @author Everyone
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