/**
 * @file built_in.c
 *
 * @brief Vsetky vstavane funkcie, ktore musi interpret podporovat
 * @author Filip
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "built-in.h"


/**
 * Pomocna funkcia, upraveny strtod
 *
 * @param string
 * @param double *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _strtod( char *input, double *result )
{
	if( input == NULL )
		return E_INTERPRET_ERROR;

	char *endptr;
	*result = strtod( input, &endptr );
	
	if( isprint( endptr[0] ) && endptr[0] != ' ' )
		return E_NUM_CAST;
	return E_OK;
}


/**
 * Pomocna funkcia, najrychlejsi sposob zistenia poctu cifier
 *
 * @param integer
 * @return Uspesnost
 */
unsigned intNumSpaces( unsigned input )
{
	unsigned n = 1, result = 0;
	while( input >= n )
	{
		n *= 10;
		result++;
	}
	return result;
}


/**
 * Vstavana funkcia, vracia bool z TERM
 *
 * @param TERM
 * @param bool *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _boolval( TERM *input, bool *result )
{
    switch( input->type )
    {
        case DATA_BOOL:
            *result = input->data._bool;
            break;
			
        case DATA_INT:
            if( input->data._int == 0 )
                *result = false;
            else
                *result = true;
            break;
			
        case DATA_DOUBLE:
            if( input->data._double == 0.0 )
                *result = false;
            else
                *result = true;
            break;
			
        case DATA_STRING:
            if( input->data._string[0] == '\0' )
                *result = false;
            else
                *result = true;
            break;
			
        case DATA_NULL:
            *result = false;
            break;
			
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia double z TERM
 *
 * @param TERM
 * @param double *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _doubleval( TERM *input, double *result )
{
    switch( input->type )
    {
        case DATA_BOOL:
            *result = (double) input->data._bool;
            break;
			
        case DATA_INT:
            *result = (double) input->data._int;
            break;
			
        case DATA_DOUBLE:
            *result = input->data._double;
            break;
			
        case DATA_STRING:
            return _strtod( input->data._string, result );
            break;
			
        case DATA_NULL:
            *result = 0.0;
            break;
			
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia int z TERM
 *
 * @param TERM
 * @param int *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _intval( TERM *input, unsigned *result )
{
    switch( input->type )
    {
        case DATA_BOOL:
            *result = input->data._bool;
            break;
			
        case DATA_INT:
            *result = input->data._int;
            break;
			
        case DATA_DOUBLE:
            if( input->data._double < (double) UINT_MAX + 1 && input->data._double >= 0.0 )
                *result = (unsigned) input->data._double;
            else
                return E_INTERPRET_ERROR;
            break;
			
        case DATA_STRING:
            *result = atoi( input->data._string );
            break;
			
        case DATA_NULL:
            *result = 0;
            break;
			
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia string z TERM
 *
 * @param TERM
 * @param char *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _strval( TERM *input, char **result )
{
	switch( input->type )
    {
        case DATA_BOOL:
			if( input->data._bool == true )
			{
				*result = malloc( 2 * sizeof( char ) );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				*result[0] = '1';
				*result[1] = '\0';
			}
			else
			{
				*result = malloc( 1 * sizeof( char ) );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				*result[0] = '\0';
			}
            break;
			
        case DATA_INT:
			if( input->data._int <= 0 )
			{
				*result = malloc( 2 * sizeof( char ) );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				*result[0] = '0';
				*result[1] = '\0';
			}
			else
			{
				*result = malloc( ( intNumSpaces( input->data._int ) * sizeof( char ) ) + 1 );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				sprintf( *result, "%u", input->data._int );
			}
            break;
			
        case DATA_DOUBLE:
			if( input->data._double <= 0 )
			{
				*result = malloc( 2 * sizeof( char ) );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				*result[0] = '0';
				*result[1] = '\0';
			}
			else
			{
				char temp[MAX_DBL_DIGITS] = { '\0', };
				sprintf( temp, "%g", input->data._double );
				if( temp[MAX_DBL_DIGITS - 1] != '\0' )
					return E_INTERPRET_ERROR;
				*result = malloc( strlen( temp ) * sizeof( char ) + 1 );
				if( *result == NULL )
					return E_INTERPRET_ERROR;
				strcpy( *result, temp );
			}
            break;
			
        case DATA_STRING:
			*result = malloc( strlen( input->data._string ) * sizeof( char ) + 1 );
			if( *result == NULL )
				return E_INTERPRET_ERROR;
			strcpy( *result, input->data._string );
            break;
			
        case DATA_NULL:
			*result = malloc( 1 * sizeof( char ) );
			if( *result == NULL )
				return E_INTERPRET_ERROR;
			*result[0] = '\0';
            break;
			
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, nacitava string zo stdin
 *
 * @param TERM
 * @param char *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _get_string( char *result )
{
    return E_OK;
}


/**
 * Vstavana funkcia, vypisuje vsetky vstupy na stdout
 *
 * @param int *( return ) - pocet uspesne vypisanych vstupov
 * @param vstupny string
 * @return Uspesnost
 */
E_ERROR_TYPE _put_string( int *result, char *input, ... )
{
    return E_OK;
}


/**
 * Vstavana funkcia, vracia podretazec zo vstupneho retazca v rozmedzi pozicii zadanych v parametroch
 *
 * @param vstupny string
 * @param zaciatocna pozicia
 * @param konecna pozicia
 * @param char *( return ) - string podla pozicii
 * @return Uspesnost
 */
E_ERROR_TYPE _get_substring( char *input, int begpos, int endpos, char *result )
{
    return E_OK;
}


/**
 * Vstavana funkcia, hlada retazec v retazci a vracia jeho poziciu
 *
 * @param retazec v ktorom funckia vyhladava
 * @param hladany retazec
 * @param int *( return ) - pozicia najdeneho retazca
 * @return Uspesnost
 */
E_ERROR_TYPE _find_string( char *input, char *find, int *result )
{
    return E_OK;
}


/**
 * Vstavana funkcia, zoraduje znaky stringu podla ich ordinalnej hodnotyh od najnizsej po najvyssiu
 *
 * @param Vstupny retazec
 * @param Zoradeny retazec
 * @return Uspesnost
{
 */
E_ERROR_TYPE _sort_string( char *input, char *result )
{
    return E_OK;
}
