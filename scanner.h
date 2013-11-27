/**
 * @file scanner.h
 *
 * @brief Header file modulu lexikalneho analyzatoru
 * @author Vladimír Čillo, xcillo00
 */

#ifndef SCANNER_H
#define SCANNER_H

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
    E_LPARENTHESES, // (
    E_RPARENTHESES, // )
    E_IDENT,        // identifikator
    E_COMA,         // ,
    E_TERM,          // term, pomocny enum pre precedencnu syntakticku analyzu
    E_LABRACK,      // {
    E_SEMICL,       // ;
    E_VAR,          // premenna
    E_INT,          // integer
    E_DOUBLE,       // double
    E_LITER,        // literal
    E_FALSE,    // ----
    E_NULL,
    E_TRUE, 
    E_WHILE,
    E_FUNCTION,
    E_IF,
    E_ELSE,
    E_RETURN,   // ----
    E_EQ,           // =
    E_RABRACK,      // }
    E_INVLD,        // je to invalid
    E_EOF, // padla
    E_LOCAL,
    R_E,            // prvok precedencnej tabulky >
    R_C,            // prvok precedencnej tabulky <
    R_N,            // prvok precedencnej tabulky no_rule
    R_P,            // prvok precedencnej tabulky =
    E_E,            // nonterminal, pouzivany v gramatike
    
} TOKEN_TYPE;

/** Stavy konecnej masinky **/
typedef enum
{
    INIT,
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
    } data;
} T_token;


/** Funkcie **/
void scanner_init( char *file_start, size_t file_size );
void scanner_get_token( T_token* token );
void print_token( T_token* token );

#endif

