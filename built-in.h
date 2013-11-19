/**
 * @file built_in.h
 *
 * @brief Hlavickovy subor pre vstavane funkcie interpretu
 * @author Filip
 */

#include "types.h"
#include "ial.h"
#include <stdbool.h>

#define INIT_STRING_SIZE 20
#define MAX_DBL_DIGITS 20

#ifndef BUILT_IN_H
#define BUILT_IN_H

/** Definicia datovych typov */
typedef enum {
    DATA_BOOL,
    DATA_INT,
    DATA_DOUBLE,
    DATA_STRING,
    DATA_NULL,
    DATA_UNDEF
} DATA_TYPE;

/** Struktura dat neznamenho typu */
typedef struct {
    DATA_TYPE type;
    union {
        int _int;
        double _double;
        bool _bool;
        char *_string;
    } data;
} TERM;

#endif //BUILT_IN_H

E_ERROR_TYPE _boolval( TERM *input, bool *result );
E_ERROR_TYPE _doubleval( TERM *input, double *result );
E_ERROR_TYPE _intval( TERM *input, unsigned *result );
E_ERROR_TYPE _strval( TERM *input, char **result );
E_ERROR_TYPE _get_string( char **result );
E_ERROR_TYPE _put_string( int *result, char *input, ... );
E_ERROR_TYPE _get_substring( char *input, int begpos, int endpos, char **result );
E_ERROR_TYPE _find_string( char *input, char *find, int *result );
E_ERROR_TYPE _sort_string( char *input, char **result );
