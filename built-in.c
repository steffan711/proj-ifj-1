/**
 * @file built_in.c
 *
 * @brief Vsetky vstavane funkcie, ktore musi interpret podporovat
 * @author Filip
 */

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
E_ERROR_TYPE _strval( TERM *input, char *result )
{
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
