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
} /** OTESTOVANE **/