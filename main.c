/**
 * @file main.c
 *
 * @brief Hlavna vetva
 * @author Vsetci
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"

// ERRNO.H ?

/**
 * Main funkcia programu
 *
 * @todo Core is missing, at least
 *
 * @param parameter info, expected to be exactly 2
 * @param parameter, expected to include name of file
 * @return Vrati uspesnost vykonania programu
 */
int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "NO PARAMETER\n" );
        // TODO: Doplnit chybove hlasenie
        return E_OTHER;
    }
    
    FILE *f;
    if ( ( f = fopen( argv[1], "r" ) ) == NULL )
    {
        fprintf( stderr, "Unable to open file: %s\n", argv[1] );
        return E_INTERPRET_ERROR;
    }
    
    /* urcenie MAXIMA velkosti pamate, ktoru je potrebne alokovat na nacitanie suboru */
    fseek( f, 0, SEEK_END );
    size_t file_size = ftell( f );
    rewind( f );
    
    char *source_file; /**< abstrakcia zdrojoveho suboru */
    
    if ( ( source_file = malloc( file_size + 1 ) ) == NULL ) //+1 kvoli znaku znaku konca retazca
    {
        fprintf( stderr, "Unable to allocate memory. Exitting ...\n" );
        fclose( f );
        return E_INTERPRET_ERROR;
    }
    
    size_t num_of_chars = fread( source_file, 1, file_size, f );
    if ( ferror( f ) )
    {
        perror( "Error" ); // TODO
        free( source_file );
        fclose( f );
        return E_INTERPRET_ERROR;
    }
    fclose( f );
    source_file[num_of_chars] = '\0';   //EOF 
    
    E_ERROR_TYPE ret_val = E_OK;
    
    if ( check_file_header( source_file ) != E_OK ) // kontrola '<?php' na zaciatku suboru
    {
        fprintf( stderr, "Invalid source file. Exitting ...\n" );
        return E_OTHER;
    }
    
    /** MAGIC HAPPENS HERE**/
    // inicializacia
    
    // zacat koleso
    
    /*E_ERROR_TYPE retval = ...// prebrat kod */
    
    //vratit
    
    
    /** END OF MAGIC **/
    
    free( source_file );
    return ret_val;
}
