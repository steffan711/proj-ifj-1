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
    return E_OK;
}


/**
 * Pomocna funkcia, najrychlejsi sposob zistenia poctu cifier
 *
 * @param integer
 * @return Uspesnost
 */
unsigned intNumSpaces( int input )
{
    int result = 1;
    while( (input = input / 10) != 0 )
    {
        result++;
    }
    return result;
} //  upravene, dochadzalo k preteceniu


/**
 * Vstavana funkcia, vracia bool z T_DVAR
 *
 * @param T_DVAR
 * @param bool *( return )
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
            
        case VAR_STRING:
        case VAR_CONSTSTRING:
            if( input[0].size > 0 )
                result->data._bool = false;
            else
                result->data._bool = true;
            break;
            
        case VAR_NULL:
            result->data._bool = false;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
} // dorobene checkovanie dlzky


/**
 * Vstavana funkcia, vracia double z T_DVAR
 *
 * @param T_DVAR
 * @param double *( return )
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
            
        case VAR_STRING:
        case VAR_CONSTSTRING:
			result->type = VAR_DOUBLE;
            return _strtod( input[0].data._string, &(result->data._double) );
            break;
            
        case VAR_NULL:
			result->type = VAR_DOUBLE;
            result->data._double = 0.0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
} // moze byt, otestujem


/**
 * Vstavana funkcia, vracia int z T_DVAR
 *
 * @param T_DVAR
 * @param int *( return )
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
            //if( input[0].data._double < (double) UINT_MAX + 1 && input[0].data._double >= 0.0 )
			{
				result->type = VAR_INT;
                result->data._int = (int) input[0].data._double;
			}
            /*else
                return E_INTERPRET_ERROR;*/
            break;
            
        case VAR_STRING:
        case VAR_CONSTSTRING:
			result->type = VAR_INT;
            if ( input[0].size > 0 )
            {
                //result->data._int = atoi( input[0].data._string ); // TODO, retazec nie je ukonceny nulou
                
                char tmp[input[0].size+1];
                memcpy(tmp, input[0].data._string, input[0].size);
                tmp[input[0].size] = 0;
                result->data._int = atoi( tmp );
            }
            else
            {
                result->data._int = 0;
            }
            break;
            
        case VAR_NULL:
			result->type = VAR_INT;
            result->data._int = 0;
            break;
            
        default:
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
} // moze byt


/**
 * Vstavana funkcia, vracia string z T_DVAR
 *
 * @param T_DVAR
 * @param char *( return )
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
				result->type = VAR_STRING;
				result->data._string = malloc( 1 );
				result->size = 1;
				
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;
                result->data._string[0] = '1';
            }
            else
            {
				result->type = VAR_STRING;
				//result->data._string = malloc( 0 );
				result->size = 0;
				
                /*if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;*/
                //result->data._string[0] = 0;
            }
            break;
            
        case VAR_INT:
				result->type = VAR_STRING;
                int n = intNumSpaces( input[0].data._int );
				result->data._string = malloc( n+2 );
                if ( input[0].data._int <= 0 )
                    result->size = n+1;
                else 
                    result->size = n;
				
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;
                    
                sprintf( result->data._string, "%d", input[0].data._int );
            break;
            
        case VAR_DOUBLE:
            {
                char temp[MAX_DBL_DIGITS] = { 0, };
                int n = sprintf( temp, "%g", input[0].data._double );
                //if( temp[MAX_DBL_DIGITS - 1] != 0 )
                if( n < 0 )
                    return E_INTERPRET_ERROR;
					
				result->type = VAR_STRING;
                result->data._string = malloc( n );
				result->size = n;
				
                if( result->data._string == NULL )
                    return E_INTERPRET_ERROR;
                memcpy( result->data._string, temp, n );
            }
            break;
            
        case VAR_STRING:
			result->type = VAR_STRING;
            result->data._string = malloc( input[0].size );
			result->size = input[0].size;
			
            if( result->data._string == NULL )
                return E_INTERPRET_ERROR;
            
            memcpy( result->data._string, input[0].data._string, input[0].size );
            //strcpy( result->data._string, input[0].data._string );
            break;
            
        case VAR_NULL:
			result->type = VAR_STRING;
            result->data._string = malloc( 0 );
			result->size = 0;
            break;
            
        default:
       
            return E_INTERPRET_ERROR;        /**< error! todo */
    }

    return E_OK;
} // moze byt


/**
 * Vstavana funkcia, nacitava string zo stdin
 *
 * @param char *( return )
 * @return Uspesnost
 */
E_ERROR_TYPE get_string( T_DVAR input[], int size, T_DVAR *result )
{
    int c = getchar(), counter = 0, max = INIT_STRING_SIZE;
    
	(void)input;
	(void)size;
	
    if( c == EOF || c == '\n' )
    {
		result->type = VAR_STRING;
		result->data._string = malloc( sizeof( char ) );;
		result->size = 1;
		
        if( result->data._string == NULL )
            return E_INTERPRET_ERROR;
        result->data._string[0] = 0;
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
    
    //help[counter] = 0;
    
	result->type = VAR_STRING;
    result->data._string = help;
	result->size = counter;
    
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
E_ERROR_TYPE put_string( T_DVAR input[], int size, T_DVAR *result )
{
	
	if( size < 1 )
		return E_PARAM;
        
	result->type = VAR_INT;
	result->data._int = size;
    
	for( int i = 0; i < size; i++ )
	{
        switch( input[i].type )
        {
            case VAR_STRING:
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
                {
                    putchar('1');
                }
                else
                {
                    /* printf("")*/
                }
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
 * @param vstupny string
 * @param zaciatocna pozicia
 * @param konecna pozicia
 * @param char *( return ) - string podla pozicii
 * @return Uspesnost
 */
E_ERROR_TYPE get_substring( T_DVAR input[], int size, T_DVAR *result )
{
	if( size != 3 )
		return E_OTHER;
		
    int inplen = strlen( input[0].data._string ),
        sublen = ( input[2].data._int - input[1].data._int ),
		begpos = input[1].data._int,
		endpos = input[2].data._int,
        counter = 0;
	
    if( begpos < 0 || endpos < 0 || begpos > endpos || begpos > inplen || endpos > inplen )
        return E_OTHER;
    
    char *help = malloc( ( sublen + 1 ) * sizeof( char ) );
    if( help == NULL )
        return E_INTERPRET_ERROR;
    
    for( int i = begpos; i < endpos; i++ )
        help[counter++] = input[0].data._string[i];
    
    help[counter] = 0;
    
	result->type = VAR_STRING;
    result->data._string = help;
	result->size = sublen + 1;
    
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
E_ERROR_TYPE find_string( T_DVAR input[], int size, T_DVAR *result )
{
	if( size != 2 )
		return E_OTHER;
	
	result->type = VAR_INT;
	result->data._int = kmpmatch( input[0].data._string, input[1].data._string );
	
    return E_OK;
}


/**
 * Vstavana funkcia, zoraduje znaky stringu podla ich ordinalnej hodnotyh od najnizsej po najvyssiu
 *
 * @param Vstupny retazec
 * @param Zoradeny retazec
 * @return Uspesnost
 */
E_ERROR_TYPE sort_string( T_DVAR input[], int size, T_DVAR *result )
{
	if( size != 1 )
		return E_OTHER;
		
	if( input[0].type != VAR_STRING )
		return E_OTHER;
	
    int len = strlen( input[0].data._string ) + 1;
    
    char *help = malloc( len * sizeof( char ) );
    help[len - 1] = 0;
    
	if( len > 1 )
	{
		strcpy( help, input[0].data._string );
		quicksort( help, 0, len - 2 );
	}
    
    result->type = VAR_STRING;
    result->size = len;
	result->data._string = help;
    
    return E_OK;
}
