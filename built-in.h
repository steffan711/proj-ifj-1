/**
 * @file built_in.h
 *
 * @brief Hlavickovy subor pre vstavane funkcie interpretu
 * @author Filip
 */

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

E_ERROR_TYPE _boolval( TERM *input, bool *result );
E_ERROR_TYPE _doubleval( TERM *input, double *result );
E_ERROR_TYPE _intval( TERM *input, unsigned *result );
E_ERROR_TYPE _strval( TERM *input, char *result );
E_ERROR_TYPE _get_string( char *result );
E_ERROR_TYPE _put_string( int *result, char *input, ... );
E_ERROR_TYPE _get_substring( char *input, int begpos, int endpos, char *result );
E_ERROR_TYPE _find_string( char *input, char *find, int *result );
E_ERROR_TYPE _sort_string( char *input, char *result );
