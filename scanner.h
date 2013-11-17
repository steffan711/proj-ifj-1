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
    E_TERM,          // term, pomocny enum pre precedencnu syntakticku analyzu
    E_LABRACK,      // {
    E_SEMICL,       // ;
    E_VAR,          // premenna
    E_INT,          // integer
    E_DOUBLE,       // double
    E_LITER,        // literal
    R_E,            // prvok precedencnej tabulky >
    R_C,            // prvok precedencnej tabulky <
    R_N,            // prvok precedencnej tabulky no_rule
    R_P,            // prvok precedencnej tabulky =
    E_E,            // nonterminal, pouzivany v gramatike
    E_EQ,           // =
    E_RABRACK,      // }
    E_COMA,         // ,
    E_IDENT,        // identifikator
    E_INVLD,        // je to invalid
        // ---- klucove slova
        E_WHILE,
        E_FUNCTION,
        E_FALSE,
        E_NULL,
        E_TRUE,
        E_IF,
        E_ELSE,
        E_RETURN,
        // ----
    E_EOF,          // padla
} TOKEN_TYPE;

/** Stavy konecnej masinky **/
typedef enum
{
    INIT,
    FINISH,         // EOF
    T_ID,           // identifikator
    T_INT,          // integer
    T_FLOAT,
    T_ASS,          // = 
    T_FRACTION,     // /
    T_BLOCK_C,      // blokovy komentar
    T_CONCAT,       // .
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

typedef enum
{
    operator_divider,
    number_divider,
} divider;          // oznacenie mnoziny znakov ktora moze nasledovat za nejakym znakom

/** Bezodny IKEA buffer - R.I.P
typedef struct _tStringBuffer	
{
    unsigned allocated_size;
    unsigned size;  // aktualny pocet znakov v retaci, ptr[size] = '\0'
    char *ptr;      // ukazatel na retazec
} tStringBuffer; **/

 /** Globalne premenne **/
 char*          current_pos;  // aktualna pozicia scannera v subore, pneprepisovat
 unsigned       scanner_line;
 const char*    file_origin; // zaciatok suboru v pamati

 
 
/** Funkcie **/
void scanner_init( char *file_start );
void scanner_get_token( T_token* token );
void print_token( T_token* token );

#end if

