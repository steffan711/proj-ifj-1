/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file scanner.h Header file lexikalneho analyzatoru
 * @author Vladimír Čillo, Štefan Martiček
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

/** Typy tokenov + terminaly a nonterminaly + symboly do precedencnej tabulky,
 ** akekolvek zmeny su prisne zakazane (zavisi na tom funkcnost tabulky)**/ 
typedef enum
{
    E_CONCAT,       // .
    E_NOT_EQ,       // !==
    E_TRIPLEEQ,     // ===
    E_PLUS,         // +
    E_MULT,         // *
    E_MINUS,        // -
    E_DIV,          // /
    E_LESS,         // <
    E_GREATER,      // >
    E_LESSEQ,       // <=
    E_GREATEREQ,    // >=
    E_OR1,          // or
    E_AND1,         // and
    E_OR2,          // ||
    E_AND2,         // &&
    E_LPARENTHESES, // (
    E_COMA,         // ,
    E_RPARENTHESES, // )
    E_IDENT,        // identifikator
    E_TERM,         // term, pomocny enum pre precedencnu syntakticku analyzu
    E_UMINUS,       // unarne minus
    E_NEG,          // !
    E_SEMICL,       // ;
    E_VAR,          // premenna
    E_INT,          // integer
    E_DOUBLE,       // double
    E_LITER,        // literal   
    E_BOOL,         // bool
    E_NULL,         // ---- klucove slova
    E_WHILE,
    E_ELSEIF,
    E_FUNCTION,
    E_IF,
    E_ELSE,
    E_RETURN,       // ---- 
    E_EQ,           // =
    E_LABRACK,      // {
    E_RABRACK,      // }
    E_INVLD,        // je to invalid
    E_EOF,          // padla
    E_LOCAL,        // logicka premenna pre semanticky analyzator
    R_E,            // prvok precedencnej tabulky >
    R_C,            // prvok precedencnej tabulky <
    R_N,            // prvok precedencnej tabulky no_rule
    R_P,            // prvok precedencnej tabulky =
    E_E,            // nonterminal, pouzivany v gramatike
} TOKEN_TYPE;

/** Stavy konecnej masinky **/
typedef enum
{
    T_ID,           // identifikator
    T_VAR,          // premenna
    T_INT,          // integer
    T_FLOAT,
    T_ASS,          // = 
    T_FRACTION,     // /
    T_BLOCK_C,      // blokovy komentar
    T_GREATER,      // >
    T_LESS,         // <
    T_EXCLAM,       // !
    T_LIT,          // literal
    T_EXP,          // exponent
} FSM_STATE;

typedef struct token
{
    TOKEN_TYPE ttype;
    unsigned line;
    unsigned length; // pocet znakov v retazci vratane ukoncovacej nuly 
    union {
        int    _int;
        double _double;
        char*  _string;
        bool   _bool;
    } data;
} T_token;


/** Funkcie **/
void scanner_init( char *file_start, unsigned file_size );
void scanner_get_token( T_token* token );
void print_token( T_token* token );

#endif

