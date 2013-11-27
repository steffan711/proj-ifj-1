/**
 * @file types.h
 *
 * @brief Hlavickovy subor s definiciami datovych typov
 * @author Everyone
 */

#ifndef TYPES_H
#define TYPES_H

int sstrcmp( const char * str1, const char * str2, int str1_size, int str2_size );

/* Definicia chybovych kodov */
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

#endif //TYPES_H
