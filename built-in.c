/**
 * @file built_in.c
 *
 * @brief Vsetky vstavane funkcie, ktore musi interpret podporovat
 * @author Filip
 */

#include "types.h"


/**
 * Vstavana funkcia, vracia bool z TERM
 *
 * @param TERM
 * @param bool *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _boolval( TERM *input, BOOL *result )
{
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
    return E_OK;
}


/**
 * Vstavana funkcia, vracia int z TERM
 *
 * @param TERM
 * @param int *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE _intval( TERM *input, int *result )
{
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
 */
E_ERROR_TYPE _sort_string( char *input, char *result )
{
    return E_OK;
}