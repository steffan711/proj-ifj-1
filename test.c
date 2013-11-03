/**
 * @file test.c
 *
 * @brief Hlavna vetva
 * @author Vsetci
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"


/**ie465re455Df54sfF
 *
 * Testovaci modul pre scanner.c
 */
int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Error: No input file specified !\n" );
        return E_OTHER;
    }

    E_ERROR_TYPE ret_val;
    char *subor;    /**< abstrakcia zdrojoveho suboru */

    ret_val = mmap_file( argv[1], &subor );

    if ( ret_val != E_OK )
    {
        printf("Nastala chyba\n");
        return ret_val;
    }
     

    T_token token;
    scanner_init(subor);
    
    while(token.ttype != E_EOF)
    {
        scanner_get_token(&token);
        print_token(&token);
        if(token.data._string != NULL && token.ttype != E_INT && token.ttype != E_DOUBLE)
            free(token.data._string);
        
    }
    
    
    free( subor );
    return ret_val;
}