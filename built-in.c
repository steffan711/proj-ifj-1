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
 * Vstavana funkcia, vracia double z Tokenu
 *
 * @param T_token
 * @param double *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE doubleval_token( T_token *input, double *result )
{
    switch( input->ttype )
    {
        case E_FALSE:
            *result = 0.0;
            
        case E_TRUE:
            *result = 1.0;
        
        case E_INT:
            *result = (double) input->data._int;
            break;
            
        case E_DOUBLE:
            *result = input->data._double;
            break;
            
        case E_LITER:
            return _strtod( input->data._string, result );
            break;
            
        case E_NULL:
            *result = 0.0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia bool z T_DVAR
 *
 * @param T_DVAR
 * @param bool *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE boolval( T_DVAR *input, bool *result )
{
    switch( input->type )
    {
        case VAR_BOOL:
            *result = input->data._bool;
            break;
            
        case VAR_INT:
            if( input->data._int == 0 )
                *result = false;
            else
                *result = true;
            break;
            
        case VAR_DOUBLE:
            if( input->data._double == 0.0 )
                *result = false;
            else
                *result = true;
            break;
            
        case VAR_STRING:
            if( input->data._string[0] == 0 )
                *result = false;
            else
                *result = true;
            break;
            
        case VAR_NULL:
            *result = false;
            break;
            
        case VAR_UNDEF:
            return E_INTERPRET_ERROR;        /**< error! todo */
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia double z T_DVAR
 *
 * @param T_DVAR
 * @param double *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE doubleval( T_DVAR *input, double *result )
{
    switch( input->type )
    {
        case VAR_BOOL:
            *result = (double) input->data._bool;
            break;
            
        case VAR_INT:
            *result = (double) input->data._int;
            break;
            
        case VAR_DOUBLE:
            *result = input->data._double;
            break;
            
        case VAR_STRING:
            return _strtod( input->data._string, result );
            break;
            
        case VAR_NULL:
            *result = 0.0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia int z T_DVAR
 *
 * @param T_DVAR
 * @param int *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE intval( T_DVAR *input, unsigned *result )
{
    switch( input->type )
    {
        case VAR_BOOL:
            *result = input->data._bool;
            break;
            
        case VAR_INT:
            *result = input->data._int;
            break;
            
        case VAR_DOUBLE:
            if( input->data._double < (double) UINT_MAX + 1 && input->data._double >= 0.0 )
                *result = (unsigned) input->data._double;
            else
                return E_INTERPRET_ERROR;
            break;
            
        case VAR_STRING:
            *result = atoi( input->data._string );
            break;
            
        case VAR_NULL:
            *result = 0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia string z T_DVAR
 *
 * @param T_DVAR
 * @param char *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE strval( T_DVAR *input, char **result )
{
    switch( input->type )
    {
        case VAR_BOOL:
            if( input->data._bool == true )
            {
                *result = malloc( 2 * sizeof( char ) );
                if( *result == NULL )
                    return E_INTERPRET_ERROR;
                *( *result ) = '1';
                *( *result + 1 ) = 0;
            }
            else
            {
                *result = malloc( 1 * sizeof( char ) );
                if( *result == NULL )
                    return E_INTERPRET_ERROR;
                *( *result ) = 0;
            }
            break;
            
        case VAR_INT:
            if( input->data._int <= 0 )
            {
                *result = malloc( 2 * sizeof( char ) );
                if( *result == NULL )
                    return E_INTERPRET_ERROR;
                *( *result ) = '0';
                *( *result + 1 ) = 0;
            }
            else
            {
                *result = malloc( ( intNumSpaces( input->data._int ) + 1 ) * sizeof( char ) );
                if( *result == NULL )
                    return E_INTERPRET_ERROR;
                sprintf( *result, "%u", input->data._int );
            }
            break;
            
        case VAR_DOUBLE:
            if( 1 )
            {
                char temp[MAX_DBL_DIGITS] = { 0, };
                sprintf( temp, "%g", input->data._double );
                if( temp[MAX_DBL_DIGITS - 1] != 0 )
                    return E_INTERPRET_ERROR;
                *result = malloc( ( strlen( temp ) + 1 ) * sizeof( char ) );
                if( *result == NULL )
                    return E_INTERPRET_ERROR;
                strcpy( *result, temp );
            }
            break;
            
        case VAR_STRING:
            *result = malloc( ( strlen( input->data._string ) + 1 ) * sizeof( char ) );
            if( *result == NULL )
                return E_INTERPRET_ERROR;
            strcpy( *result, input->data._string );
            break;
            
        case VAR_NULL:
            *result = malloc( 1 * sizeof( char ) );
            if( *result == NULL )
                return E_INTERPRET_ERROR;
            *( *result ) = 0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
}


/**
 * Vstavana funkcia, nacitava string zo stdin
 *
 * @param char *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE get_string( char **result )
{    
    int c = getchar(), counter = 0, max = INIT_STRING_SIZE;
        
    if( c == EOF || c == '\n' )
    {
        *result = malloc( sizeof( char ) );
        if( *result == NULL )
            return E_INTERPRET_ERROR;
        *( *result ) = 0;
        return E_OK;
    }

    char *help = malloc( INIT_STRING_SIZE * sizeof( char ) );
    if( help == NULL )
        return E_INTERPRET_ERROR;
    
    while( c != '\n' && c != EOF )
    {
        if( counter < ( max - 1 ) )
            help[counter] = c;
        else
        {
            max *= 2;
            help = realloc( help, max );
            if( help == NULL )
                return E_INTERPRET_ERROR;
            help[counter] = c;
        }
        
        counter++;
        c = getchar();
    }
    
    help[counter] = 0;
    
    *result = help;
    
    return E_OK;
}


/**
 * Vstavana funkcia, vypisuje vsetky vstupy na stdout
 *
 * @param *T_DVAR[] - ukazatel na pole dynamickych premennych
 * @param int - pocet poloziek v poli
 * @param int *( return ) - pocet uspesne vypisanych vstupov
 * @return Uspesnost
 */
E_ERROR_TYPE put_string( T_DVAR *input[], int count, int *result )
{

    // WAITING FOR EXACT IMPLEMENTATION REQUIREMENTS...
    
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
E_ERROR_TYPE get_substring( char *input, int begpos, int endpos, char **result )
{
    int inplen = strlen( input ),
        sublen = ( endpos - begpos ),
        counter = 0;
    
    if( begpos < 0 || endpos < 0 || begpos > endpos || begpos > inplen || endpos > inplen )
        return E_OTHER;
    
    char *help = malloc( ( sublen + 1 ) * sizeof( char ) );
    if( help == NULL )
        return E_INTERPRET_ERROR;
    
    for( int i = begpos; i < endpos; i++ )
        help[counter++] = input[i];
    
    help[counter] = 0;
    
    *result = help;
    
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
E_ERROR_TYPE find_string( char *input, char *find, int *result )
{
    if( find[0] == 0 )
        *result = 0;
    
    return E_OK;
}


/**
 * Vstavana funkcia, zoraduje znaky stringu podla ich ordinalnej hodnotyh od najnizsej po najvyssiu
 *
 * @param Vstupny retazec
 * @param Zoradeny retazec
 * @return Uspesnost
 */
E_ERROR_TYPE sort_string( char *input, char **result )
{
    int len = strlen( input ) + 1;
    
    char *help = malloc( len * sizeof( char ) );
    help[len - 1] = 0;
    
    strcpy( help, input );

    quicksort( help, 0, len - 1 );
    
    *result = help;
    
    return E_OK;
}
