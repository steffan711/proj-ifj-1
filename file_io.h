/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file file_io.h Header file pre operacie so suborom + makra getc, ungetc
 * @author Oliver Nemček
 */

#include "types.h"
#include <stdio.h>
#include <stdlib.h>

/* Abstrakcia suboru nacitaneho v pamati */
#define _FILE_IN_MEMORY_
#ifdef _FILE_IN_MEMORY_
  #undef getc
  #undef ungetc
  #undef EOF
  #define getc( x ) ( *((x)++) )
  #define ungetc( x ) (x--)
  #define EOF 0
#endif


E_ERROR_TYPE check_file_header( char **source_file );
E_ERROR_TYPE mmap_file(const char* filename, char **file_pointer, unsigned* file_size);
