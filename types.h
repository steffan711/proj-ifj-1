/**
 * @file types.h
 *
 * @brief Hlavickovy subor s definiciami datovych typov
 * @author Everyone
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

/** Definicia chybovych kodov */
typedef enum {
    E_OK, 
    E_LEX,                  /**< chyba lex. analyzy */
    E_SYNTAX,               /**< chyba syntaktickej analyzy */
    E_SEM,                  /**< semanticka chyba */
    E_PARAM,                /**< nespravny parameter funkcie */
    E_UNDEF_VAR,            /**< nedefinovana premenna */
    E_ZERO_DIV = 10,        /**< delenie nulou */
    E_NUM_CAST,             /**< pretypovanie na cislo (doubleval) */
    E_INCOMPATIBLE,         /**< typova kompatibilita v arit. a relacnych vyrazoch */
    E_OTHER,                /**< ostatne chyby */
    E_INTERPRET_ERROR = 99  /**< interna chyba interpretu  - napr malloc, ... */
} E_ERROR_TYPE;

/** Definicia datovych typov */
typedef enum {
    DATA_BOOL,
    DATA_INT,
    DATA_DOUBLE,
    DATA_STRING,
    DATA_NULL,
    DATA_UNDEF
} DATA_TYPE;

/** Struktura dat neznamenho typu */
typedef struct {
    DATA_TYPE type;
    union {
        int _int;
        double _double;
        bool _bool;
        char *_string;
    } data;
} TERM;

#endif //TYPES_H
