/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file main.c Modul lexikalneho analyzatoru
 * @author Vladimír Čillo, Oliver Nemček, Štefan Martiček, Filip Ilavský
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"
#include "syntax.h"
#include "generator.h"
#include "ial.h"
#include "runtime.h"

int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Error: No input file specified !\n" );
        return E_OTHER;
    }

    E_ERROR_TYPE ret_val;
    char *handle_subor;    /**< abstrakcia zdrojoveho suboru */
    unsigned file_size;       /**< velkost suboru */
    
    ret_val = mmap_file( argv[1], &handle_subor, &file_size );

    if ( ret_val != E_OK )
        return E_INTERPRET_ERROR;
    
    char *subor = handle_subor;
    
    if ( check_file_header( &subor ) != E_OK ) // kontrola '<?php' na zaciatku suboru
    {
        fprintf( stderr, "Error on line 1: Missing '<?php' inserted. \n" );
        free(handle_subor);
        return E_SYNTAX;
    }
 
    scanner_init( subor, file_size - 5); // scanner dostava subor o 5 znakov mensi koli '<?php'
    
    if ( ( ret_val = GeneratorInit( ) ) != E_OK )
    {
        GeneratorErrorCleanup( );
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
        RuntimeErrorCleanup();
    
    GeneratorDeleteTapes(ptr);
    free( handle_subor );
    return ret_val;
}



