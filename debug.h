/**
 * @file debug.h
 *
 * @brief Header file pre debugovacie funkcie
 * @author Vsetci
 */
 
#ifndef DEBUG_H
#define DEBUG_H
/** Hlavicky funkcii **/

extern const char const *TOKEN_NAME[];
extern const char const *OPCODE_NAME[];
void print_token( T_token* token );


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