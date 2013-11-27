/**
 * @file file_io.h
 *
 * @brief Hlavickovy subor pre vstup/vystup funkcie s makrami, nahradzujucimi zakladne stdio funkcie
 * @author Vsetci
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
E_ERROR_TYPE mmap_file(const char* filename, char **file_pointer, size_t* file_size);
