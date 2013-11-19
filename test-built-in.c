#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "types.h"
#include "built-in.h"


/** boolval test */
void TEST1()
{
    bool res = false;
    TERM term;
    
    term.type = DATA_BOOL;
    
        term.data._bool = true;
        _boolval( &term, &res );
        printf( "bool(true)     -> bool: %d\n", res );
        
        term.data._bool = false;
        _boolval( &term, &res );
        printf( "bool(false)    -> bool: %d\n", res );
    
    term.type = DATA_INT;
    
        term.data._int = 0;
        if( _boolval( &term, &res ) == E_OK )
            printf( "int(0)         -> bool: %d\n", res );
        else
            printf( "int(0)         -> bool: error\n" );
        
        term.data._int = 1;
        if( _boolval( &term, &res ) == E_OK )
            printf( "int(1)         -> bool: %d\n", res );
        else
            printf( "int(1)         -> bool: error\n" );
        
        term.data._int = 2;
        if( _boolval( &term, &res ) == E_OK )
            printf( "int(2)         -> bool: %d\n", res );
        else
            printf( "int(2)         -> bool: error\n" );
    
    term.type = DATA_DOUBLE;
    
        term.data._double = 0.0;
        if( _boolval( &term, &res ) == E_OK )
            printf( "double(0.0)    -> bool: %d\n", res );
        else
            printf( "double(0.0)    -> bool: error\n" );
        
        term.data._double = 1.0;
        if( _boolval( &term, &res ) == E_OK )
            printf( "double(1.0)    -> bool: %d\n", res );
        else
            printf( "double(1.0)    -> bool: error\n" );
        
        term.data._double = 1.2;
        if( _boolval( &term, &res ) == E_OK )
            printf( "double(1.2)    -> bool: %d\n", res );
        else
            printf( "double(1.2)    -> bool: error\n" );

    term.data._string = malloc( 2 * sizeof( char ) );
    term.type = DATA_STRING;
    
        term.data._string[0] = 0;
        term.data._string[1] = 0;
        if( _boolval( &term, &res ) == E_OK )
            printf( "string(\"\")     -> bool: %d\n", res );
        else
            printf( "string(\"\")     -> bool: error\n" );
        
        term.data._string[0] = '1';
        term.data._string[1] = 0;
        if( _boolval( &term, &res ) == E_OK )
            printf( "string(\"1\")    -> bool: %d\n", res );
        else
            printf( "string(\"1\")    -> bool: error\n" );
        
        term.data._string[0] = '1';
        term.data._string[1] = '1';
        if( _boolval( &term, &res ) == E_OK )
            printf( "string(\"11\")   -> bool: %d\n", res );
        else
            printf( "string(\"11\")   -> bool: error\n" );
			
	free( term.data._string );
}
/** /boolval test */


/** intval test */
void TEST2()
{
    unsigned res = 0;
    TERM term;
    
    term.type = DATA_BOOL;
    
        term.data._bool = true;
        _intval( &term, &res );
        printf( "bool(true)     -> int: %d\n", res );
        
        term.data._bool = false;
        _intval( &term, &res );
        printf( "bool(false)    -> int: %d\n", res );
    
    term.type = DATA_INT;
    
        term.data._int = 0;
        if( _intval( &term, &res ) == E_OK )
            printf( "int(0)         -> int: %d\n", res );
        else
            printf( "int(0)         -> int: error\n" );
        
        term.data._int = 1;
        if( _intval( &term, &res ) == E_OK )
            printf( "int(1)         -> int: %d\n", res );
        else
            printf( "int(1)         -> int: error\n" );
        
        term.data._int = 2;
        if( _intval( &term, &res ) == E_OK )
            printf( "int(2)         -> int: %d\n", res );
        else
            printf( "int(2)         -> int: error\n" );
    
    term.type = DATA_DOUBLE;
    
        term.data._double = 0.0;
        if( _intval( &term, &res ) == E_OK )
            printf( "double(0.0)    -> int: %d\n", res );
        else
            printf( "double(0.0)    -> int: error\n" );
        
        term.data._double = 1.0;
        if( _intval( &term, &res ) == E_OK )
            printf( "double(1.0)    -> int: %d\n", res );
        else
            printf( "double(1.0)    -> int: error\n" );
        
        term.data._double = 1.2;
        if( _intval( &term, &res ) == E_OK )
            printf( "double(1.2)    -> int: %d\n", res );
        else
            printf( "double(1.2)    -> int: error\n" );

    term.data._string = malloc( 2 * sizeof( char ) );
    term.type = DATA_STRING;
    
        term.data._string[0] = 0;
        term.data._string[1] = 0;
        if( _intval( &term, &res ) == E_OK )
            printf( "string(\"\")     -> int: %d\n", res );
        else
            printf( "string(\"\")     -> int: error\n" );
        
        term.data._string[0] = '1';
        term.data._string[1] = 0;
        if( _intval( &term, &res ) == E_OK )
            printf( "string(\"1\")    -> int: %d\n", res );
        else
            printf( "string(\"1\")    -> int: error\n" );
        
        term.data._string[0] = 'e';
        term.data._string[1] = 0;
        if( _intval( &term, &res ) == E_OK )
            printf( "string(\"e\")    -> int: %d\n", res );
        else
            printf( "string(\"e\")    -> int: error\n" );
			
	free( term.data._string );
}
/** /intval test */


/** doubleval test */
void TEST3()
{
    double res = 0;
    TERM term;
    
    term.type = DATA_BOOL;
    
        term.data._bool = true;
        _doubleval( &term, &res );
        printf( "bool(true)     -> double: %f\n", res );
        
        term.data._bool = false;
        _doubleval( &term, &res );
        printf( "bool(false)    -> double: %f\n", res );
    
    term.type = DATA_INT;
    
        term.data._int = 0;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "int(0)         -> double: %f\n", res );
        else
            printf( "int(0)         -> double: error\n" );
        
        term.data._int = 1;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "int(1)         -> double: %f\n", res );
        else
            printf( "int(1)         -> double: error\n" );
        
        term.data._int = 2;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "int(2)         -> double: %f\n", res );
        else
            printf( "int(2)         -> double: error\n" );
    
    term.type = DATA_DOUBLE;
    
        term.data._double = 0.0;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "double(0.0)    -> double: %f\n", res );
        else
            printf( "double(0.0)    -> double: error\n" );
        
        term.data._double = 1.0;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "double(1.0)    -> double: %f\n", res );
        else
            printf( "double(1.0)    -> double: error\n" );
        
        term.data._double = 1.2;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "double(1.2)    -> double: %f\n", res );
        else
            printf( "double(1.2)    -> double: error\n" );

    term.data._string = malloc( 6 * sizeof( char ) );
    term.type = DATA_STRING;
    
        term.data._string[0] = 0;
        term.data._string[1] = 0;
        term.data._string[2] = 0;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "string(\"\")     -> double: %f\n", res );
        else
            printf( "string(\"\")     -> double: error\n" );
        
        term.data._string[0] = 'c';
        term.data._string[1] = 'a';
        if( _doubleval( &term, &res ) == E_OK )
            printf( "string(\"ca\")   -> double: %f\n", res );
        else
            printf( "string(\"ca\")   -> double: error\n" );
        
        term.data._string[0] = '4';
        term.data._string[1] = 'e';
        if( _doubleval( &term, &res ) == E_OK )
            printf( "string(\"4e\")   -> double: %f\n", res );
        else
            printf( "string(\"4e\")   -> double: error\n" );
		
		term.data._string[0] = '4';
		term.data._string[1] = 'e';
		term.data._string[2] = '1';
        term.data._string[3] = ' ';
        term.data._string[4] = 's';
        term.data._string[5] = 0;
        if( _doubleval( &term, &res ) == E_OK )
            printf( "string(\"4e1 s\")-> double: %f\n", res );
        else
            printf( "string(\"4e1 s\")-> double: error\n" );
	
	free( term.data._string );
}
/** /doubleval test */


/** strval test */
void TEST4()
{
    char *res = NULL;
    TERM term;
    
    term.type = DATA_BOOL;
    
        term.data._bool = true;
        _strval( &term, &res );
        printf( "bool(true)     -> string: %s\n", res );
		if( res != NULL )
			free( res );
        
        term.data._bool = false;
        _strval( &term, &res );
        printf( "bool(false)    -> string: %s\n", res );
		if( res != NULL )
			free( res );
    
    term.type = DATA_INT;
    
        term.data._int = 0;
        if( _strval( &term, &res ) == E_OK )
            printf( "int(0)         -> string: %s\n", res );
        else
            printf( "int(0)         -> string: error\n" );
        if( res != NULL )
			free( res );

        term.data._int = 1;
        if( _strval( &term, &res ) == E_OK )
            printf( "int(1)         -> string: %s\n", res );
        else
            printf( "int(1)         -> string: error\n" );
        if( res != NULL )
			free( res );
			
        term.data._int = 2;
        if( _strval( &term, &res ) == E_OK )
            printf( "int(2)         -> string: %s\n", res );
        else
            printf( "int(2)         -> string: error\n" );
		if( res != NULL )
			free( res );
			
    term.type = DATA_DOUBLE;
    
        term.data._double = 0.0;
        if( _strval( &term, &res ) == E_OK )
            printf( "double(0.0)    -> string: %s\n", res );
        else
            printf( "double(0.0)    -> string: error\n" );
        if( res != NULL )
			free( res );
			
        term.data._double = 1.0;
        if( _strval( &term, &res ) == E_OK )
            printf( "double(1.0)    -> string: %s\n", res );
        else
            printf( "double(1.0)    -> string: error\n" );
		if( res != NULL )
			free( res );

        term.data._double = 1.2;
        if( _strval( &term, &res ) == E_OK )
            printf( "double(1.2)    -> string: %s\n", res );
        else
            printf( "double(1.2)    -> string: error\n" );
		if( res != NULL )
			free( res );

    term.data._string = malloc( 6 * sizeof( char ) );
    term.type = DATA_STRING;
    
        term.data._string[0] = 0;
        term.data._string[1] = 0;
		term.data._string[2] = 0;
        if( _strval( &term, &res ) == E_OK )
            printf( "string(\"\")     -> string: %s\n", res );
        else
            printf( "string(\"\")     -> string: error\n" );
        if( res != NULL )
			free( res );

        term.data._string[0] = 'c';
        term.data._string[1] = 'a';
        if( _strval( &term, &res ) == E_OK )
            printf( "string(\"ca\")   -> string: %s\n", res );
        else
            printf( "string(\"ca\")   -> string: error\n" );
		if( res != NULL )
			free( res );
        
        term.data._string[0] = '4';
        term.data._string[1] = 'e';
        if( _strval( &term, &res ) == E_OK )
            printf( "string(\"4e\")   -> string: %s\n", res );
        else
            printf( "string(\"4e\")   -> string: error\n" );
		if( res != NULL )
			free( res );
			
		term.data._string[0] = '4';
		term.data._string[1] = 'e';
		term.data._string[2] = '1';
        term.data._string[3] = ' ';
        term.data._string[4] = 's';
		term.data._string[5] = '\0';
        if( _strval( &term, &res ) == E_OK )
            printf( "string(\"4e1 s\")-> string: %s\n", res );
        else
            printf( "string(\"4e1 s\")-> string: error\n" );
		if( res != NULL )
			free( res );

	free( term.data._string );
}
/** /strval test */


/** sort_string test */
void TEST5()
{
	char *res = NULL;
	char *slovo = malloc( sizeof( char ) * 20 );
	slovo[0] = 'v';
	slovo[1] = 'r';
	slovo[2] = 'z';
	slovo[3] = 'l';
	slovo[4] = 'o';
	slovo[5] = 'j';
	slovo[6] = 'm';
	slovo[7] = 'l';
	slovo[8] = 't';
	slovo[9] = 'j';
	slovo[10] = 'i';
	slovo[11] = 'g';
	slovo[12] = 'g';
	slovo[13] = 'f';
	slovo[14] = 'e';
	slovo[15] = 'u';
	slovo[16] = 's';
	slovo[17] = 'a';
	slovo[18] = 'b';
	slovo[19] = '\0';
	
		if( _sort_string( slovo, &res ) == E_OK )
            printf( "sort_string(\"%s\")-> string: %s\n", slovo, res );
        else
            printf( "string(\"%s\")-> string: error\n", slovo );
		if( res != NULL )
			free( res );
			
	slovo[0] = '\0';
	slovo[1] = '\0';
	slovo[2] = '\0';
	slovo[3] = '\0';
	slovo[4] = '\0';
	slovo[5] = '\0';
	slovo[6] = '\0';
	slovo[7] = '\0';
	slovo[8] = '\0';
	slovo[9] = '\0';
	slovo[10] = '\0';
	slovo[11] = '\0';
	slovo[12] = '\0';
	slovo[13] = '\0';
	slovo[14] = '\0';
	slovo[15] = '\0';
	slovo[16] = '\0';
	slovo[17] = '\0';
	slovo[18] = '\0';
	slovo[19] = '\0';
	
		if( _sort_string( slovo, &res ) == E_OK )
            printf( "sort_string(\"%s\")-> string: %s\n", slovo, res );
        else
            printf( "string(\"%s\")-> string: error\n", slovo );
		if( res != NULL )
			free( res );
			
	free( slovo );
}
/** /sort_string test */



int main()
{	
    printf( "\n--------------TEST1 (_boolval) BEGIN!!\n\n" );
    TEST1();
    printf( "\n--------------TEST1 (_boolval) END!!\n" );
    printf( "\n--------------TEST2 (_intval) BEGIN!!\n\n" );
    TEST2();
    printf( "\n--------------TEST2 (_intval) END!!\n" );
	printf( "\n--------------TEST3 (_doubleval) BEGIN!!\n\n" );
    TEST3();
    printf( "\n--------------TEST3 (_doubleval) END!!\n" );
	printf( "\n--------------TEST4 (_strval) BEGIN!!\n\n" );
    TEST4();
    printf( "\n--------------TEST4 (_strval) END!!\n" );
	printf( "\n--------------TEST5 (_sort_string) BEGIN!!\n\n" );
    TEST5();
    printf( "\n--------------TEST5 (_sort_string) END!!\n" );
    return 0;
}