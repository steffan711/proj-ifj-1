/**
 * @file debug.h
 *
 * @brief Header file pre debugovacie funkcie
 * @author Vsetci
 */
 
#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>
#include "scanner.h"
#include "generator.h"


/** Hlavicky funkcii **/


extern const char const *TOKEN_NAME[];
extern const char const *OPCODE_NAME[];
void print_token( T_token* token );
void ERROR( char *MSB, ... );
void print_char( FILE *file, char *text, unsigned int size );
void print_DVAR(T_DVAR *ptr);


/* DEBUG */

#ifdef DEBUG 
    #define PRINTD(...) \
        do { \
            fprintf ( stderr, __VA_ARGS__ ); \
        } \
        while(0)
#else
    #define PRINTD(...) \
        do { \
        ; \
        } \
        while(0)
#endif

/* END DEBUG */


#endif

