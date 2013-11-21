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
    size_t file_size;       /**< velkost suboru */
    
    ret_val = mmap_file( argv[1], &handle_subor, &file_size );

    if ( ret_val != E_OK )
        return E_INTERPRET_ERROR;
    
    char *subor = handle_subor;
    
    if ( check_file_header( &subor ) != E_OK ) // kontrola '<?php' na zaciatku handle_suboru
    {
        fprintf( stderr, "Invalid source file. Exiting ...\n" );
        free(handle_subor);
        return E_OTHER;
    }
 
    
    scanner_init( subor, file_size - 6); // scanner dostava subor o 6 znakov mensi koli '<?php '
    
    check_syntax();
    
    free( handle_subor );
    return 0;
}



