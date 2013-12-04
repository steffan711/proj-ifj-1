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
#include "debug.h"


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

    if( endptr > input )
        if( (--endptr)[0] == '.' )
            return E_NUM_CAST;
        
    return E_OK;
}


/**
 * Pomocna funkcia, najrychlejsi sposob zistenia poctu cifier
 *
 * @param integer
 * @return integer
 */
unsigned intNumSpaces( int input )
{
    int result = 1;
    while( (input = input / 10) != 0 )
    {
        result++;
    }
    return result;
}


/**
 * Vstavana funkcia, vracia bool z T_DVAR
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE boolval( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 1 )
        return E_OTHER;
        
    result->type = VAR_BOOL;
    
    switch( input[0].type )
    {
        case VAR_BOOL:
            result->data._bool = input[0].data._bool;
            break;
            
        case VAR_INT:
            if( input[0].data._int == 0 )
                result->data._bool = false;
            else
                result->data._bool = true;
            break;
            
        case VAR_DOUBLE:
            if( input[0].data._double == 0.0 )
                result->data._bool = false;
            else
                result->data._bool = true;
            break;
            
        case VAR_CONSTSTRING:
            if( input[0].size > 0 )
                result->data._bool = true;
            else
                result->data._bool = false;
            break;
            
        case VAR_NULL:
            result->data._bool = false;
            break;
            
        default:
            return E_UNDEF_VAR;
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia double z T_DVAR
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE doubleval( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 1 )
        return E_OTHER;

    switch( input[0].type )
    {
        case VAR_BOOL:
            result->type = VAR_DOUBLE;
            result->data._double = (double) input[0].data._bool;
            break;
            
        case VAR_INT:
            result->type = VAR_DOUBLE;
            result->data._double = (double) input[0].data._int;
            break;
            
        case VAR_DOUBLE:
            result->type = VAR_DOUBLE;
            result->data._double = input[0].data._double;
            break;
            
        case VAR_CONSTSTRING:
            {
                result->type = VAR_DOUBLE;
                if( input[0].size == 0 )
                {
                    result->data._double = 0.0;
                    return E_OK;
                }
                char temp[input[0].size + 1];
                temp[input[0].size] = 0;
                memcpy( temp, input[0].data._string, input[0].size );
                return _strtod( temp, &(result->data._double) );
            }
            break;
            
        case VAR_NULL:
            result->type = VAR_DOUBLE;
            result->data._double = 0.0;
            break;
            
        default:
            return E_INTERPRET_ERROR;
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia int z T_DVAR
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE intval( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 1 )
        return E_OTHER;
    
    switch( input[0].type )
    {
        case VAR_BOOL:
            result->type = VAR_INT;
            result->data._int = input[0].data._bool;
            break;
            
        case VAR_INT:
            result->type = VAR_INT;
            result->data._int = input[0].data._int;
            break;
            
        case VAR_DOUBLE:
            result->type = VAR_INT;
            result->data._int = (int) input[0].data._double;
            break;
            
        case VAR_CONSTSTRING:
            result->type = VAR_INT;
            if ( input[0].size > 0 )
            {
                char temp[input[0].size+1];
                memcpy( temp, input[0].data._string, input[0].size );
                temp[input[0].size] = 0;
                result->data._int = atoi( temp );
            }
            else
                result->data._int = 0;
            break;
            
        case VAR_NULL:
            result->type = VAR_INT;
            result->data._int = 0;
            break;
            
        default:
            return E_INTERPRET_ERROR;
    }

    return E_OK;
}


/**
 * Vstavana funkcia, vracia string z T_DVAR
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE strval( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 1 )
        return E_OTHER;
    switch( input[0].type )
    {
        case VAR_BOOL:
            if( input[0].data._bool == true )
            {
                result->data._string = malloc( 1 );
                result->size = 1;
                
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;
                result->type = VAR_STRING;
                result->data._string[0] = '1';
            }
            else
            {
                result->type = VAR_CONSTSTRING;
                result->size = 0;
            }
            break;
            
        case VAR_INT:
            if( input[0].data._int == 0 )
            {
                result->type = VAR_STRING;
                result->size = 1;
                result->data._string = malloc( 1 );
                result->data._string[0] = '0';
            }
            else
            {
                
                int n = intNumSpaces( input[0].data._int );
                result->data._string = malloc( n + 2 );
                if ( input[0].data._int <= 0 )
                    result->size = n + 1;
                else
                    result->size = n;
                
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;
                result->type = VAR_STRING;   
                sprintf( result->data._string, "%d", input[0].data._int );
            }
            break;
        case VAR_DOUBLE:
            {
                char temp[MAX_DBL_DIGITS] = { 0, };
                int n = sprintf( temp, "%g", input[0].data._double );
                if( n < 0 )
                    return E_INTERPRET_ERROR;

                result->data._string = malloc( n );
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;

                result->size = n;
                result->type = VAR_STRING;
                memcpy( result->data._string, temp, n );
            }
            break;
            
        case VAR_CONSTSTRING:
            if( input[0].size == 0 )
            {
                *result = input[0];
                input[0].type = VAR_UNDEF; // optimalizacia, parametre su nanovo mallocovane, takze sa nestane konflikt
            }
            
            result->type = VAR_STRING;
            result->size = input[0].size;
            result->data._string = malloc( input[0].size );
            if( result->data._string == NULL )
                return E_INTERPRET_ERROR;
            memcpy( result->data._string, input[0].data._string, input[0].size );
            break;
            
        case VAR_NULL:
            result->type = VAR_CONSTSTRING;
            result->size = 0;
            break;
            
        default:
            return E_OTHER;
    }

    return E_OK;
}


/**
 * Vstavana funkcia, nacitava string zo stdin
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE get_string( T_DVAR input[], int size, T_DVAR *result )
{
    int c = getchar(), counter = 0, max = INIT_STRING_SIZE;
    
    (void)input;
    (void)size;
    
    if( c == EOF || c == '\n' )
    {
        result->type = VAR_CONSTSTRING;
        result->size = 0;
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
            char *tmp = help;
            help = realloc( help, max );
            if( help == NULL )
            {
                free( tmp );
                return E_INTERPRET_ERROR;
            }
            help[counter] = c;
        }
        
        counter++;
        c = getchar();
    }
    
    result->type = VAR_STRING;
    result->data._string = help;
    result->size = counter;
    
    return E_OK;
}


/**
 * Vracia dlzku retazca zadaneho prvym parametrom
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE runtime_strlen( T_DVAR input[], int size, T_DVAR *result )
{
    result->type = VAR_INT;
    ( void ) size;
    
    switch( input[0].type )
    {
        case VAR_CONSTSTRING:   
            result->data._int = input[0].size;
            break;

        case VAR_BOOL :
            result->data._int = ( input[0].data._bool == true ) ? 1 : 0;
            break;
            
        case VAR_DOUBLE :
        {
            char temp[MAX_DBL_DIGITS + 1]; 
            result->data._int = sprintf( temp, "%g", input[0].data._double );
            break;
        }

        case VAR_INT :
            result->data._int = (signed) intNumSpaces(input[0].data._int);
            break;
            
        case VAR_NULL :
            result->data._int = 0;
            break;
            
        default:
            return E_OTHER;

    }

    return E_OK;
}


/**
 * Vstavana funkcia, vypisuje vsetky retazce na stdout
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE put_string( T_DVAR input[], int size, T_DVAR *result )
{
    result->type = VAR_INT;
    result->data._int = size;
    
    for( int i = 0; i < size; i++ )
    {
        switch( input[i].type )
        {
            case VAR_CONSTSTRING:
                print_char( stdout, input[i].data._string, input[i].size );
                break;
            case VAR_INT:
                printf("%d", input[i].data._int );
                break;
            case VAR_DOUBLE:
                printf("%g", input[i].data._double );
                break;
            case VAR_NULL:
                break;
            case VAR_BOOL:
                if( input[i].data._bool == true )
                    putchar('1');
                break;
            default:
                return E_OTHER;
        }
    }
    return E_OK;
}


/**
 * Vstavana funkcia, vracia podretazec zo vstupneho retazca v rozmedzi pozicii zadanych v parametroch
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE get_substring( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 3 )
        return E_OTHER;
    
    if( input[0].type != VAR_CONSTSTRING )
    {
        return E_OTHER;
    }
    if( input[1].type != VAR_INT && input[2].type != VAR_INT )
    {
        return E_OTHER;
    }
    
    int inplen = input[0].size,
        sublen = ( input[2].data._int - input[1].data._int ),
        begpos = input[1].data._int,
        endpos = input[2].data._int,
        counter = 0;
    
    if( begpos < 0 || endpos < 0 || begpos > endpos || begpos > inplen || endpos > inplen )
        return E_OTHER;
    
    if( sublen == 0 )
    {
        result->type = VAR_CONSTSTRING;
        result->data._string = NULL;
        result->size = 0;
        return E_OK;
    }
    
    char *help = malloc( sublen * sizeof( char ) );
    if( help == NULL )
        return E_INTERPRET_ERROR;
    
    for( int i = begpos; i < endpos; i++ )
        help[counter++] = input[0].data._string[i];
    
    result->type = VAR_STRING;
    result->data._string = help;
    result->size = sublen;
    
    return E_OK;
}


/**
 * Vstavana funkcia, hlada retazec v retazci a vracia jeho poziciu
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE find_string( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 2 )
        return E_OTHER;
    if( input[0].type != VAR_CONSTSTRING )
    {
        return E_OTHER;
    }
    if( input[1].type != VAR_CONSTSTRING )
    {
        return E_OTHER;
    }
    
    result->type = VAR_INT;
    result->data._int = kmpmatch( input[0].data._string, input[0].size, input[1].data._string, input[1].size );
    
    return E_OK;
}


/**
 * Vstavana funkcia, zoraduje znaky stringu podla ich ordinalnej hodnotyh od najnizsej po najvyssiu
 *
 * @param T_DVAR[] vstupne parametre
 * @param integer pocet vstup parametrov
 * @param T_DVAR vystup
 * @return Uspesnost
 */
E_ERROR_TYPE sort_string( T_DVAR input[], int size, T_DVAR *result )
{
    if( size != 1 || input[0].type != VAR_CONSTSTRING )
        return E_OTHER;
    
    char *help;
    result->size = input[0].size;
    
    if( input[0].size >= 1 )
    {
        result->type = VAR_STRING;
        help = malloc( input[0].size );
        memcpy( help, input[0].data._string, input[0].size );
        if( input[0].size > 1 )
            quicksort( help, 0, input[0].size - 1 );
        result->data._string = help;
    }
    else
    {
        result->type = VAR_CONSTSTRING;
    }
    
    return E_OK;
}
