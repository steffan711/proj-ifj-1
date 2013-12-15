/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file test-built-in.c - Testovaci modul vstavanych funkcii
 * @author Filip Ilavský
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "types.h"
#include "built-in.h"
#include "generator.h"


/** boolval test */
void TEST1()
{
    T_DVAR res;
    T_DVAR term[1];
    
    term[0].type = VAR_BOOL;
    
        term[0].data._bool = true;
        boolval( term, 1, &res );
        printf( "bool(true)     -> bool: %d\n", res.data._bool );
        
        term[0].data._bool = false;
        boolval( term, 1, &res );
        printf( "bool(false)    -> bool: %d\n", res.data._bool );
    
    term[0].type = VAR_INT;
    
        term[0].data._int = 0;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "int(0)         -> bool: %d\n", res.data._bool );
        else
            printf( "int(0)         -> bool: error\n" );
        
        term[0].data._int = 1;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "int(1)         -> bool: %d\n", res.data._bool );
        else
            printf( "int(1)         -> bool: error\n" );
        
        term[0].data._int = 2;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "int(2)         -> bool: %d\n", res.data._bool );
        else
            printf( "int(2)         -> bool: error\n" );
    
    term[0].type = VAR_DOUBLE;
    
        term[0].data._double = 0.0;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "double(0.0)    -> bool: %d\n", res.data._bool );
        else
            printf( "double(0.0)    -> bool: error\n" );
        
        term[0].data._double = 1.0;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "double(1.0)    -> bool: %d\n", res.data._bool );
        else
            printf( "double(1.0)    -> bool: error\n" );
        
        term[0].data._double = 1.2;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "double(1.2)    -> bool: %d\n", res.data._bool );
        else
            printf( "double(1.2)    -> bool: error\n" );

    term[0].data._string = malloc( 2 * sizeof( char ) );
    term[0].type = VAR_STRING;
    
        term[0].data._string[0] = 0;
        term[0].data._string[1] = 0;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "string(\"\")     -> bool: %d\n", res.data._bool );
        else
            printf( "string(\"\")     -> bool: error\n" );
        
        term[0].data._string[0] = '1';
        term[0].data._string[1] = 0;
        if( boolval( term, 1, &res ) == E_OK )
            printf( "string(\"1\")    -> bool: %d\n", res.data._bool );
        else
            printf( "string(\"1\")    -> bool: error\n" );
        
        term[0].data._string[0] = '1';
        term[0].data._string[1] = '1';
        if( boolval( term, 1, &res ) == E_OK )
            printf( "string(\"11\")   -> bool: %d\n", res.data._bool );
        else
            printf( "string(\"11\")   -> bool: error\n" );
			
	free( term[0].data._string );
}
/** /boolval test */


/** intval test */
void TEST2()
{
    T_DVAR res;
    T_DVAR term[1];
    
    term[0].type = VAR_BOOL;
    
        term[0].data._bool = true;
        intval( term, 1, &res );
        printf( "bool(true)     -> int: %d\n", res.data._int );
        
        term[0].data._bool = false;
        intval( term, 1, &res );
        printf( "bool(false)    -> int: %d\n", res.data._int );
    
    term[0].type = VAR_INT;
    
        term[0].data._int = 0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "int(0)         -> int: %d\n", res.data._int );
        else
            printf( "int(0)         -> int: error\n" );
        
        term[0].data._int = 1;
        if( intval( term, 1, &res ) == E_OK )
            printf( "int(1)         -> int: %d\n", res.data._int );
        else
            printf( "int(1)         -> int: error\n" );
        
        term[0].data._int = 2;
        if( intval( term, 1, &res ) == E_OK )
            printf( "int(2)         -> int: %d\n", res.data._int );
        else
            printf( "int(2)         -> int: error\n" );
    
    term[0].type = VAR_DOUBLE;
    
        term[0].data._double = 0.0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "double(0.0)    -> int: %d\n", res.data._int );
        else
            printf( "double(0.0)    -> int: error\n" );
        
        term[0].data._double = 1.0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "double(1.0)    -> int: %d\n", res.data._int );
        else
            printf( "double(1.0)    -> int: error\n" );
        
        term[0].data._double = 1.2;
        if( intval( term, 1, &res ) == E_OK )
            printf( "double(1.2)    -> int: %d\n", res.data._int );
        else
            printf( "double(1.2)    -> int: error\n" );

    term[0].data._string = malloc( 2 * sizeof( char ) );
    term[0].type = VAR_STRING;
    
        term[0].data._string[0] = 0;
        term[0].data._string[1] = 0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "string(\"\")     -> int: %d\n", res.data._int );
        else
            printf( "string(\"\")     -> int: error\n" );
        
        term[0].data._string[0] = '1';
        term[0].data._string[1] = 0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "string(\"1\")    -> int: %d\n", res.data._int );
        else
            printf( "string(\"1\")    -> int: error\n" );
        
        term[0].data._string[0] = 'e';
        term[0].data._string[1] = 0;
        if( intval( term, 1, &res ) == E_OK )
            printf( "string(\"e\")    -> int: %d\n", res.data._int );
        else
            printf( "string(\"e\")    -> int: error\n" );
			
	free( term[0].data._string );
}
/** /intval test */


/** doubleval test */
void TEST3()
{
    T_DVAR res;
    T_DVAR term[1];
    
    term[0].type = VAR_BOOL;
    
        term[0].data._bool = true;
        doubleval( term, 1, &res );
        printf( "bool(true)     -> double: %f\n", res.data._double );
        
        term[0].data._bool = false;
        doubleval( term, 1, &res );
        printf( "bool(false)    -> double: %f\n", res.data._double );
    
    term[0].type = VAR_INT;
    
        term[0].data._int = 0;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "int(0)         -> double: %f\n", res.data._double );
        else
            printf( "int(0)         -> double: error\n" );
        
        term[0].data._int = 1;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "int(1)         -> double: %f\n", res.data._double );
        else
            printf( "int(1)         -> double: error\n" );
        
        term[0].data._int = 2;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "int(2)         -> double: %f\n", res.data._double );
        else
            printf( "int(2)         -> double: error\n" );
    
    term[0].type = VAR_DOUBLE;
    
        term[0].data._double = 0.0;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "double(0.0)    -> double: %f\n", res.data._double );
        else
            printf( "double(0.0)    -> double: error\n" );
        
        term[0].data._double = 1.0;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "double(1.0)    -> double: %f\n", res.data._double );
        else
            printf( "double(1.0)    -> double: error\n" );
        
        term[0].data._double = 1.2;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "double(1.2)    -> double: %f\n", res.data._double );
        else
            printf( "double(1.2)    -> double: error\n" );

    term[0].data._string = malloc( 6 * sizeof( char ) );
    term[0].type = VAR_STRING;
    
        term[0].data._string[0] = 0;
        term[0].data._string[1] = 0;
        term[0].data._string[2] = 0;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "string(\"\")     -> double: %f\n", res.data._double );
        else
            printf( "string(\"\")     -> double: error\n" );
        
        term[0].data._string[0] = 'c';
        term[0].data._string[1] = 'a';
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "string(\"ca\")   -> double: %f\n", res.data._double );
        else
            printf( "string(\"ca\")   -> double: error\n" );
        
        term[0].data._string[0] = '4';
        term[0].data._string[1] = 'e';
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "string(\"4e\")   -> double: %f\n", res.data._double );
        else
            printf( "string(\"4e\")   -> double: error\n" );
		
		term[0].data._string[0] = '4';
		term[0].data._string[1] = 'e';
		term[0].data._string[2] = '1';
        term[0].data._string[3] = ' ';
        term[0].data._string[4] = 's';
        term[0].data._string[5] = 0;
        if( doubleval( term, 1, &res ) == E_OK )
            printf( "string(\"4e1 s\")-> double: %f\n", res.data._double );
        else
            printf( "string(\"4e1 s\")-> double: error\n" );
	
	free( term[0].data._string );
}
/** /doubleval test */


/** strval test */
void TEST4()
{
    T_DVAR res;
    T_DVAR term[1];
    
    term[0].type = VAR_BOOL;
    
        term[0].data._bool = true;
        strval( term, 1, &res );
        printf( "bool(true)     -> string: %s\n", res.data._string );
		if( res.data._string != NULL )
			free( res.data._string );
        
        term[0].data._bool = false;
        strval( term, 1, &res );
        printf( "bool(false)    -> string: %s\n", res.data._string );
		if( res.data._string != NULL )
			free( res.data._string );
    
    term[0].type = VAR_INT;
    
        term[0].data._int = 0;
        if( strval( term, 1, &res ) == E_OK )
            printf( "int(0)         -> string: %s\n", res.data._string );
        else
            printf( "int(0)         -> string: error\n" );
        if( res.data._string != NULL )
			free( res.data._string );

        term[0].data._int = 1;
        if( strval( term, 1, &res ) == E_OK )
            printf( "int(1)         -> string: %s\n", res.data._string );
        else
            printf( "int(1)         -> string: error\n" );
        if( res.data._string != NULL )
			free( res.data._string );
			
        term[0].data._int = 2;
        if( strval( term, 1, &res ) == E_OK )
            printf( "int(2)         -> string: %s\n", res.data._string );
        else
            printf( "int(2)         -> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );
			
    term[0].type = VAR_DOUBLE;
    
        term[0].data._double = 0.0;
        if( strval( term, 1, &res ) == E_OK )
            printf( "double(0.0)    -> string: %s\n", res.data._string );
        else
            printf( "double(0.0)    -> string: error\n" );
        if( res.data._string != NULL )
			free( res.data._string );
			
        term[0].data._double = 1.0;
        if( strval( term, 1, &res ) == E_OK )
            printf( "double(1.0)    -> string: %s\n", res.data._string );
        else
            printf( "double(1.0)    -> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );

        term[0].data._double = 1.2;
        if( strval( term, 1, &res ) == E_OK )
            printf( "double(1.2)    -> string: %s\n", res.data._string );
        else
            printf( "double(1.2)    -> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );

    term[0].data._string = malloc( 6 * sizeof( char ) );
    term[0].type = VAR_STRING;
    
        term[0].data._string[0] = 0;
        term[0].data._string[1] = 0;
		term[0].data._string[2] = 0;
        if( strval( term, 1, &res ) == E_OK )
            printf( "string(\"\")     -> string: %s\n", res.data._string );
        else
            printf( "string(\"\")     -> string: error\n" );
        if( res.data._string != NULL )
			free( res.data._string );

        term[0].data._string[0] = 'c';
        term[0].data._string[1] = 'a';
        if( strval( term, 1, &res ) == E_OK )
            printf( "string(\"ca\")   -> string: %s\n", res.data._string );
        else
            printf( "string(\"ca\")   -> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );
        
        term[0].data._string[0] = '4';
        term[0].data._string[1] = 'e';
        if( strval( term, 1, &res ) == E_OK )
            printf( "string(\"4e\")   -> string: %s\n", res.data._string );
        else
            printf( "string(\"4e\")   -> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );
			
		term[0].data._string[0] = '4';
		term[0].data._string[1] = 'e';
		term[0].data._string[2] = '1';
        term[0].data._string[3] = ' ';
        term[0].data._string[4] = 's';
		term[0].data._string[5] = '\0';
        if( strval( term, 1, &res ) == E_OK )
            printf( "string(\"4e1 s\")-> string: %s\n", res.data._string );
        else
            printf( "string(\"4e1 s\")-> string: error\n" );
		if( res.data._string != NULL )
			free( res.data._string );

	free( term[0].data._string );
}
/** /strval test */


/** sort_string test */
void TEST5()
{
	T_DVAR res;
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
	T_DVAR term[1];
	
	term[0].type = VAR_STRING;
	term[0].data._string = slovo;
	term[0].size = 20;
	
		if( sort_string( term, 1, &res ) == E_OK )
            printf( "sort_string(\"%s\")-> string: %s\n", slovo, res.data._string );
        else
            printf( "string(\"%s\")-> string: error\n", slovo );
		if( res.data._string != NULL )
			free( res.data._string );
			
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
	
		if( sort_string( term, 1, &res ) == E_OK )
            printf( "sort_string(\"%s\")-> string: %s\n", slovo, res.data._string );
        else
            printf( "string(\"%s\")-> string: error\n", slovo );
		if( res.data._string != NULL )
			free( res.data._string );
	
	free( slovo );
}
/** /sort_string test */


/** get_string test */
void TEST6()
{
	T_DVAR res;
	T_DVAR term[1];
	
	if( get_string( term, 0, &res ) == E_OK && res.type == VAR_STRING )
	{
		printf( "Napisane -> size: %d -> string: \"%s\"\n", res.size, res.data._string );
		free( res.data._string );
	}
	else
		printf( "Reading from stdio FAILED!!\n" );
}
/** /get_string test */


/** put_string test */
void TEST7()
{
	int num = 10;
	T_DVAR res;
	T_DVAR term[num];
	char *temp = "nieco si napisem ->";
	int size = strlen( temp );
	for( int i = 0; i < num; i++ )
	{
		term[i].type = VAR_STRING;
		term[i].data._string = malloc( ( size + 1 ) * sizeof( char ) );
		term[i].size = size + 1;
		strcpy( term[i].data._string, temp );
	}
	
	if( put_string( term, num, &res ) == E_OK )
		printf("\nPRINT WENT WELL\n");
	else
		printf("\nPRINT CRASHED\n");
	
	for( int i = 0; i < num; i++ )
	{
		free( term[i].data._string );
	}
}
/** /put_string test */


/** get_substring test */
void TEST8()
{
	T_DVAR res;
	T_DVAR term[3];
	char *temp = "nieco si napisem ->";
	int size = strlen( temp );
	
	term[0].type = VAR_STRING;
	term[1].type = VAR_INT;
	term[2].type = VAR_INT;
	
	term[0].data._string = malloc( ( size + 1 ) * sizeof( char ) );
	term[1].data._int = 1;
	term[2].data._int = 1;
	strcpy( term[0].data._string, temp );
	
	if( get_substring( term, 3, &res ) == E_OK )
		printf( "string(\"%s\") substr( %d, %d ) ==> \"%s\"", temp, term[1].data._int, term[2].data._int, res.data._string );
	else
		printf( "string(\"%s\") substr( %d, %d ) ==> error", temp, term[1].data._int, term[2].data._int );

	free( term[0].data._string );
	free( res.data._string );
}
/** /get_substring test */


/** find_string test */
void TEST9()
{
	T_DVAR res;
	T_DVAR term[2];
	char *temp1 = "niec volaco nieco";
	char *temp2 = "";
	term[0].type = VAR_STRING;
	term[1].type = VAR_STRING;
	term[0].size = 18;
	term[1].size = 2;
	term[0].data._string = temp1;
	term[1].data._string = temp2;
	
	if( find_string( term, 2, &res ) == E_OK )
		printf( "find_string( \"%s\", \"%s\" ) ==> found on position: %d\n", term[0].data._string, term[1].data._string, res.data._int );
	else
		printf( "find_string( \"%s\", \"%s\" ) ==> error\n", term[0].data._string, term[1].data._string );
	
	char *temp3 = "vol";
	term[1].data._string = temp3;
	if( find_string( term, 2, &res ) == E_OK )
		printf( "find_string( \"%s\", \"%s\" ) ==> found on position: %d\n", term[0].data._string, term[1].data._string, res.data._int );
	else
		printf( "find_string( \"%s\", \"%s\" ) ==> error\n", term[0].data._string, term[1].data._string );
	
	char *temp4 = "volr";
	term[1].data._string = temp4;
	if( find_string( term, 2, &res ) == E_OK )
		printf( "find_string( \"%s\", \"%s\" ) ==> found on position: %d\n", term[0].data._string, term[1].data._string, res.data._int );
	else
		printf( "find_string( \"%s\", \"%s\" ) ==> error\n", term[0].data._string, term[1].data._string );
}
/** /find_string test */


int main()
{	
    printf( "\n--------------TEST1 (boolval) BEGIN!!\n\n" );
    TEST1();
    printf( "\n--------------TEST1 (boolval) END!!\n" );
	printf( "\n--------------TEST2 (intval) BEGIN!!\n\n" );
	TEST2();
	printf( "\n--------------TEST2 (intval) END!!\n" );
	printf( "\n--------------TEST3 (doubleval) BEGIN!!\n\n" );
	TEST3();
	printf( "\n--------------TEST3 (doubleval) END!!\n" );
	printf( "\n--------------TEST4 (strval) BEGIN!!\n\n" );
	TEST4();
	printf( "\n--------------TEST4 (strval) END!!\n" );
	printf( "\n--------------TEST5 (sort_string) BEGIN!!\n\n" );
	TEST5();
	printf( "\n--------------TEST5 (sort_string) END!!\n" );
	printf( "\n--------------TEST6 (get_string) BEGIN!!\n\nNie toto nieje infinite loop, toto je cakanie na vstup ukonceny EOF/\\n ;)\n" );
	TEST6();
	printf( "\n--------------TEST6 (get_string) END!!\n" );
	printf( "\n--------------TEST7 (put_string) BEGIN!!\n\n" );
	TEST7();
	printf( "\n--------------TEST7 (put_string) END!!\n" );
	printf( "\n--------------TEST8 (get_substring) BEGIN!!\n\n" );
	TEST8();
	printf( "\n--------------TEST8 (get_substring) END!!\n" );
	printf( "\n--------------TEST9 (find_string) BEGIN!!\n\n" );
	TEST9();
	printf( "\n--------------TEST9 (find_string) END!!\n" );
    return 0;
}