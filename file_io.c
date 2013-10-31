/**
 * @file file_io.c
 *
 * @brief Vsetky funckie pracujuce s real/abstraktnym suborom
 * @author Vsetci
 */

#include "types.h"
#include "file_io.h"
#include <ctype.h>


/**
 * Pomocna funkcia, zistuje validitu suboru podla vzoru
 *
 * @param Abstraktny vstupny subor
 * @return Uspesnost
 */
E_ERROR_TYPE check_file_header( char *source_file )
{
    char pattern[] = "<?php";
    for ( unsigned int i = 0; i < sizeof( pattern ) - 1; i++ ) // -1, lebo pole je dlhsie o null terminator
    {
        if ( getc( source_file ) != pattern[i] )
        {
            return E_OTHER;
        }
    }
    if ( !isspace( getc( source_file ) ) )
    {
        return E_OTHER;
    }
    return E_OK;
} 

/**
 * Funkcia mapuje(kopiruje) subor do pamate
 *
 * @param1 meno suboru so zdrojovym kodom
 * @param2 (OUT) Ukazatel na prvy znak v subore, v pripade chyby NULL;
 * @return Navratovy kod
 */
 
E_ERROR_TYPE mmap_file(const char *filename, char **file_pointer)
{
    FILE *f;
    if ( ( f = fopen(filename, "r" ) ) == NULL )
    {
        fprintf( stderr, "Unable to open file: %s\n", filename );
		*file_pointer = NULL;
        return E_INTERPRET_ERROR;
    }
    
    /* urcenie MAXIMA velkosti pamate, ktoru je potrebne alokovat na nacitanie suboru */
    fseek( f, 0, SEEK_END );
    size_t file_size = ftell( f );
    rewind( f );
   
    char *source_file;
   
    if ( ( source_file = malloc( file_size + 1 ) ) == NULL ) //+1 kvoli znaku znaku konca retazca
    {
        fprintf( stderr, "Unable to allocate memory. Exiting ...\n" );
        fclose( f );
		source_file = NULL;
        return E_INTERPRET_ERROR;
    }
    
    size_t num_of_chars = fread( source_file, 1, file_size, f );
    if ( ferror( f ) )
    {
        perror( "Error" ); // TODO
        free( source_file );
        fclose( f );
		source_file = NULL;
        return E_INTERPRET_ERROR;
    }
    fclose( f );
    source_file[num_of_chars] = '\0';   //EOF 
    
    if ( check_file_header( source_file ) != E_OK ) // kontrola '<?php' na zaciatku suboru
    {
        fprintf( stderr, "Invalid source file. Exiting ...\n" );
		source_file = NULL;
        return E_OTHER;
    }
	
	*file_pointer = source_file;
	
	return E_OK;
}
