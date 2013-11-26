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

E_ERROR_TYPE doubleval_token( T_token *input, double *result );

E_ERROR_TYPE boolval( T_DVAR *input, bool *result );
E_ERROR_TYPE doubleval( T_DVAR *input, double *result );
E_ERROR_TYPE intval( T_DVAR *input, unsigned *result );
E_ERROR_TYPE strval( T_DVAR *input, char **result );

E_ERROR_TYPE get_string( char **result );
E_ERROR_TYPE put_string( T_DVAR *input[], int count, int *result );

E_ERROR_TYPE get_substring( char *input, int begpos, int endpos, char **result );
E_ERROR_TYPE find_string( char *input, char *find, int *result );
E_ERROR_TYPE sort_string( char *input, char **result );

#endif //BUILT_IN_H