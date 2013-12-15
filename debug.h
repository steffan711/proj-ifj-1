/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file scanner.h Header file pre ladice funkcie
 * @author Vladimír Čillo, Oliver Nemček, Štefan Martiček, Filip Ilavský
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
void ERROR( const char *MSB, ... );
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

#ifdef TIME_CHECKER
#include <sys/times.h>
#include <sys/unistd.h>

extern int pocet_tikov_za_sekundu;
extern struct tms casova_struktura;
extern clock_t zaciatok_merania_tikov;
extern clock_t koniec_merania_tikov;

/** makra na presne odmeranie doby behu programu len jedneho useku v danom module */

#define ZACIATOK_MERANIA()\
do {\
    pocet_tikov_za_sekundu = sysconf(_SC_CLK_TCK);\
    times(&casova_struktura);\
    zaciatok_merania_tikov = casova_struktura.tms_utime;\
} while (0)

#define KONIEC_MERANIA()\
do {\
    times(&casova_struktura);\
    koniec_merania_tikov = casova_struktura.tms_utime;\
    printf("\x1b[32m DOBA BEHU DANEJ CASTI PROGRAMU BOLA:   \x1b[33m%f [sec]\x1b[0m\n",\
    (koniec_merania_tikov - zaciatok_merania_tikov) / (double) pocet_tikov_za_sekundu);\
} while (0)
#endif //TIME_CHECKER
/* END DEBUG */
#endif

    
