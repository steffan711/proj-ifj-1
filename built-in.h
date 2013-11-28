/**
 * @file built_in.h
 *
 * @brief Hlavickovy subor pre vstavane funkcie interpretu
 * @author Filip
 */

#include "types.h"
#include "generator.h"
#include "scanner.h"
#include "ial.h"
#include <stdbool.h>

#define INIT_STRING_SIZE 20
#define MAX_DBL_DIGITS 20

#ifndef BUILT_IN_H
#define BUILT_IN_H

E_ERROR_TYPE doubleval_token( T_token *input, T_DVAR *result );

E_ERROR_TYPE boolval( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE doubleval( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE intval( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE strval( T_DVAR input[], int size, T_DVAR *result );

E_ERROR_TYPE get_string( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE put_string( T_DVAR input[], int size, T_DVAR *result );

E_ERROR_TYPE runtime_strlen( T_DVAR input[], int size, T_DVAR *result );

E_ERROR_TYPE get_substring( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE find_string( T_DVAR input[], int size, T_DVAR *result );
E_ERROR_TYPE sort_string( T_DVAR input[], int size, T_DVAR *result );

#endif //BUILT_IN_H