/**
 * @file main.c
 *
 * @brief Hlavna vetva
 * @author Vsetci
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"
#include "syntax.h"

int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Error: No input file specified !\n" );
        return E_OTHER;
    }

    E_ERROR_TYPE ret_val;
    char *handle_subor;    /**< abstrakcia zdrojoveho handle_suboru */

    ret_val = mmap_file( argv[1], &handle_subor );

    if ( ret_val != E_OK )
        return 2;la
    
    char *subor = handle_subor;
    
    if ( check_file_header( &subor ) != E_OK ) // kontrola '<?php' na zaciatku handle_suboru
    {
        fprintf( stderr, "Invalid source file. Exiting ...\n" );
        free(handle_subor);
        return E_OTHER;
    }
    
    
    T_token token;
    token.ttype = E_INVLD;
    
    scanner_init(subor);
        
        
    printf("---------------------------");
    while(token.ttype != E_EOF)
    {
        scanner_get_token(&token);
        print_token(&token);
    }
    printf("---------------------------\n");
    
    free( handle_subor );
    return 0;
}



