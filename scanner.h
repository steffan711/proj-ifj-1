/**
 * @file scanner.h
 *
 * @brief Header file modulu lexikalneho analyzatoru
 * @author Vladimír Čillo, xcillo00
 */

#include <stdbool.h>
#define allocation_coeficient 2
#define pre_allocation 32


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
    E_BOOL,         // bool
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
    E_MALLOC,       // posral sa malloc, balime
    E_EOF          // padla
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
    T_ESCAPE,       // escape sekvencia v retazci
} FSM_STATE;

typedef struct token
{
    TOKEN_TYPE ttype;
    unsigned line;
    unsigned column;
    union {
        bool   _bool;
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

/** Bezodny IKEA buffer **/
typedef struct _tStringBuffer	
{
    unsigned allocated_size;
    unsigned size;  // aktualny pocet znakov v retaci, ptr[size] = '\0'
    char *ptr;      // ukazatel na retazec
} tStringBuffer;

 /** Globalne premenne **/
 char*          current_pos;  // aktualna pozicia scannera v subore, prepisovat len ked je to nutne
 unsigned       scanner_line, scanner_column;
 const char*    file_origin;
 //tStringBuffer stack;   // zasobnik znakov na retazce
 tStringBuffer Buffer;
 unsigned lex_length; // dlzka poslednej spracovanej lexemy, ak by niekoho nahodou zaujimalo, aka je dlha...
 
/** Funkcie **/
E_ERROR_TYPE buffer_push( char znak );
E_ERROR_TYPE scanner_init( char *file_start );
void buffer_init();
void scanner_shutdown();
void print_token( T_token* token );

void scanner_get_token( T_token* token );
/** pozn. pre volajuceho
            if( token.ttype == E_LIT) 
                free( token.data._string )
**/
