/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file file_io.c Modul pre operacie so suborom
 * @author Štefan Martiček, Vladimír Čillo, Oliver Nemček
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
E_ERROR_TYPE check_file_header( char **source_file )
{
    char pattern[] = "<?php";
    for ( unsigned int i = 0; i < sizeof( pattern ) - 1; i++ ) // -1, lebo pole je dlhsie o null terminator
    {
        if ( getc( *source_file ) != pattern[i] )
            return E_OTHER;
    }
    
    if ( !isspace( getc( *source_file ) ) )
        return E_OTHER;
    (*source_file)--;
    return E_OK;
} 

/**
 * Funkcia mapuje(kopiruje) subor do pamati
 *
 * @param1 [in]  filename       meno suboru so zdrojovym kodom
 * @param2 [out] file_pointer   ukazatel na prvy znak v subore, v pripade chyby NULL;
 * @param3 [out] file_size      velkost suboru
 * @return uspesnost
 */
 
E_ERROR_TYPE mmap_file(const char *filename, char **file_pointer, unsigned *file_size)
{
    FILE *f;
    if ( ( f = fopen(filename, "r" ) ) == NULL )
    {
        fprintf( stderr, "Error: Unable to open file: %s\n", filename );
		*file_pointer = NULL;
        return E_INTERPRET_ERROR;
    }
    
    /* urcenie MAXIMA velkosti pamate, ktoru je potrebne alokovat na nacitanie suboru */
    fseek( f, 0, SEEK_END );
    *file_size = ftell( f );
    rewind( f );
   
    char *source_file;
   
    if ( ( source_file = malloc( *file_size + 1 ) ) == NULL ) //+1 kvoli znaku znaku konca retazca
    {
        fprintf( stderr, "Error: Unable to allocate memory.\n" );
        fclose( f );
		source_file = NULL;
        return E_INTERPRET_ERROR;
    }
    
    size_t num_of_chars = fread( source_file, 1, *file_size, f );
    if ( ferror( f ) )
    {
        fprintf( stderr, "File I/O error.\n" );
        free( source_file );
        fclose( f );
		source_file = NULL;
        return E_INTERPRET_ERROR;
    }
    fclose( f );
    source_file[num_of_chars] = '\0';   //EOF 
	*file_pointer = source_file;
	
	return E_OK;
}
