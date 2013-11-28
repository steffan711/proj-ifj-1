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
#include "generator.h"
#include "runtime.h"

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
 
    scanner_init( subor, file_size - 5); // scanner dostava subor o 5 znakov mensi koli '<?php'
    
    if ( ( ret_val = GeneratorInit( ) ) != E_OK )
    {
        free( handle_subor );
        return ret_val;
    }
    
    if ( ( ret_val = check_syntax() ) != E_OK )
    {   
        GeneratorErrorCleanup( );
        free( handle_subor );
        return ret_val;
    }
    
    struct InstructionTapeBuffer *ptr;
    if ( ( ret_val = GeneratorPrepareTape( &ptr ) ) != E_OK )
    {
        GeneratorErrorCleanup( );
        free( handle_subor );
        return ret_val;
    }
    
    /* Vsetko je pripravene na beh */
    
    if ( ( ret_val = InterpretCode( ptr->array[0] ) ) != E_OK  )
    {
        RuntimeErrorCleanup();
        GeneratorDeleteTapes(ptr);
        free( handle_subor );
        return ret_val;
    }
    GeneratorDeleteTapes(ptr);
    
    
    free( handle_subor );
    return ret_val;
}



