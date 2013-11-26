/**
 * @file syntax.c
 *
 * @brief implementacia syntaktickej analyzy
 * @author Stevo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "scanner.h"
#include "expressions.h"
#include "syntax.h"
#include "generator.h"

//#define TESTY

//#define OLD

#ifdef OLD
/** PDA - pushdown automaton, konstanta velkosti a velkosti zmeny PDA */
#define SIZEOF_PDASTACK 2   //minimum je 2

static struct {
    TOKEN_TYPE *data;
    unsigned size;
    unsigned top;
} PDAStack = { .data = NULL, .size = SIZEOF_PDASTACK, .top = 0 };

/** nonterminaly PDA */
enum NONTERM_TYPES {
    ST_LIST,
    PAR_LIST
};

/** protypy funkcii */
extern inline E_ERROR_TYPE PDAStackInit ( void );
extern inline TOKEN_TYPE PDAStackTop ( void );
extern inline void PDAStackPop ( void );
extern inline E_ERROR_TYPE PDAStackPush ( TOKEN_TYPE type );
extern inline void PDAStackFree( void );
void while_treat ( void );
void if_treat ( void );
void function_treat ( void );
void return_treat ( void );
void var_ident_treat ( void );
void eof_treat ( void );
void rabrack_treat ( void );
void par_ident_treat ( void );
void function_without_par ( void );

/** special LL table */

void ( * LLtab[2][36] ) ( void ) = { 

    [ST_LIST][E_CONCAT] = NULL, [ST_LIST][E_NOT_EQ] = NULL, [ST_LIST][E_TRIPLEEQ] = NULL,
    [ST_LIST][E_PLUS] = NULL, [ST_LIST][E_MULT] = NULL, [ST_LIST][E_MINUS] = NULL,
    [ST_LIST][E_DIV] = NULL, [ST_LIST][E_LESS] = NULL, [ST_LIST][E_GREATER] = NULL,
    [ST_LIST][E_LESSEQ] = NULL, [ST_LIST][E_GREATEREQ] = NULL, [ST_LIST][E_LPARENTHESES] = NULL,
    [ST_LIST][E_RPARENTHESES] = NULL, [ST_LIST][E_IDENT] = NULL, [ST_LIST][E_COMA] = NULL,
    [ST_LIST][E_LABRACK] = NULL, [ST_LIST][E_SEMICL] = NULL, [ST_LIST][E_INT] = NULL, 
    [ST_LIST][E_DOUBLE] = NULL, [ST_LIST][E_LITER] = NULL, [ST_LIST][E_FALSE] = NULL,
    [ST_LIST][E_NULL] = NULL, [ST_LIST][E_TRUE] = NULL, [ST_LIST][E_EQ] = NULL,
    [ST_LIST][E_INVLD] = NULL, [ST_LIST][E_ELSE] = NULL,
    
    //priradenie obsluznych ukazatelov podla pravidiel
    [ST_LIST][E_VAR] = var_ident_treat,
    [ST_LIST][E_WHILE] = while_treat,
    [ST_LIST][E_FUNCTION] = function_treat, 
    [ST_LIST][E_IF] = if_treat, 
    [ST_LIST][E_RETURN] = return_treat,
    [ST_LIST][E_RABRACK] = rabrack_treat,    
    [ST_LIST][E_EOF] = eof_treat,
    
    /*---------------------------------------------------------------------------------------*/
    
    [PAR_LIST][E_CONCAT] = NULL, [PAR_LIST][E_NOT_EQ] = NULL, [PAR_LIST][E_TRIPLEEQ] = NULL,
    [PAR_LIST][E_PLUS] = NULL, [PAR_LIST][E_MULT] = NULL, [PAR_LIST][E_MINUS] = NULL,
    [PAR_LIST][E_DIV] = NULL, [PAR_LIST][E_LESS] = NULL, [PAR_LIST][E_GREATER] = NULL,
    [PAR_LIST][E_LESSEQ] = NULL, [PAR_LIST][E_GREATEREQ] = NULL, [PAR_LIST][E_EOF] = NULL,
    [PAR_LIST][E_IDENT] = NULL, [PAR_LIST][E_LABRACK] = NULL, [PAR_LIST][E_SEMICL] = NULL,
    [PAR_LIST][E_INT] = NULL, [PAR_LIST][E_DOUBLE] = NULL, [PAR_LIST][E_LITER] = NULL,
    [PAR_LIST][E_WHILE] = NULL, [PAR_LIST][E_FUNCTION] = NULL, [PAR_LIST][E_IF] = NULL,
    [PAR_LIST][E_ELSE] = NULL, [PAR_LIST][E_RETURN] = NULL, [PAR_LIST][E_FALSE] = NULL,
    [PAR_LIST][E_NULL] = NULL, [PAR_LIST][E_TRUE] = NULL, [PAR_LIST][E_EQ] = NULL,
    [PAR_LIST][E_RABRACK] = NULL, [PAR_LIST][E_INVLD] = NULL, [PAR_LIST][E_LPARENTHESES] = NULL,
    [PAR_LIST][E_COMA] = NULL,
    
    //priradenie obsluznych ukazatelov podla pravidiel
    [PAR_LIST][E_VAR] = par_ident_treat,
    [PAR_LIST][E_RPARENTHESES] = function_without_par,
    
};

/** globalne premenne */
static enum NONTERM_TYPES nonterminal;
static T_token token;
static E_ERROR_TYPE error_code;
static unsigned par_counter;

void while_treat ( void )
{
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LPARENTHESES)
    {
        if ( ( error_code = evaluate_expr( &token, E_RPARENTHESES ) ) != E_OK ) 
        {
            return;
        }
    }
    else
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LABRACK)
    {
        error_code = PDAStackPush( E_WHILE );
        #ifdef TESTY
            printf("WHILE - right syntax\n");
        #endif
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void if_treat ( void )
{
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LPARENTHESES)
    {
        if ( ( error_code = evaluate_expr( &token, E_RPARENTHESES ) ) != E_OK ) 
        {
            return;
        }
    }
    else
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LABRACK)
    {
        error_code = PDAStackPush( E_IF );
        #ifdef TESTY
            printf("IF - right syntax\n");
        #endif        
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void function_treat ( void )
{
    if ( PDAStackTop( ) != E_EOF )
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_IDENT)
    {
        //call function to add to func table
    }
    else
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LPARENTHESES)
    {
        nonterminal = PAR_LIST;
        par_counter = 0;
        #ifdef TESTY
            printf("FUNCTION - right syntax\n");
        #endif        
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void return_treat ( void )
{
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    error_code = evaluate_expr( &token, E_SEMICL );
    
    #ifdef TESTY
        printf("RETURN\n");
    #endif
}

void var_ident_treat ( void )
{
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_EQ)
    {
        #ifdef TESTY
            printf("EXPRESSION ASSIGN TO A VARIABLE - right syntax\n");
        #endif
        scanner_get_token( &token );
    
        if ( token.ttype == E_INVLD ) 
        {
            error_code = E_LEX;
            return;
        }
        
        error_code = evaluate_expr( &token, E_SEMICL );
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void eof_treat ( void )
{
    if ( PDAStackTop( ) == E_EOF )
    {
        #ifdef TESTY
            printf("EOF - right syntax\n");
        #endif
        error_code = E_OK;
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void rabrack_treat ( void )
{
    if ( PDAStackTop( ) == E_EOF )
    {
        #ifdef TESTY
            printf("END OF NOTHING - not right syntax\n");
        #endif
        error_code = E_SYNTAX;
    }
    else if ( PDAStackTop( ) == E_IF )
    {
        scanner_get_token( &token );
        
        if ( token.ttype == E_INVLD ) 
        {
            error_code = E_LEX;
            return;
        }
        
        if ( token.ttype != E_ELSE)
        {
            error_code = E_SYNTAX;
            return;
        }
        
        scanner_get_token( &token );
        
        if ( token.ttype == E_INVLD ) 
        {
            error_code = E_LEX;
            return;
        }
        
        if ( token.ttype != E_LABRACK)
        {
            error_code = E_SYNTAX;
        }
        else
        {
            #ifdef TESTY
                printf("ELSE - right syntax\n");
            #endif
            PDAStack.data[PDAStack.top] = E_ELSE;   //nechcelo sa mi kvoli tomu pisat zapuzdrovaciu funkciu
            error_code = E_OK;
        }
    }
    else
    {
        PDAStackPop( );
        error_code = E_OK;
    }
}

void par_ident_treat ( void )
{
    //tu mam v strukture token parameter volanie semantickeho
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_COMA)
    {
        #ifdef TESTY
            printf("NEXT PARAM MUST FOLLOW - right syntax\n");
        #endif
        error_code = E_OK;
        par_counter++;
        return;
    }
    else if ( token.ttype != E_RPARENTHESES )
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LABRACK)
    {
        nonterminal = ST_LIST;
        error_code = PDAStackPush( E_FUNCTION );
        #ifdef TESTY
            printf("FUNCTION PARAMS - right syntax\n");
        #endif
    }
    else
    {
        error_code = E_SYNTAX;
    }
}

void function_without_par ( void )
{
    if ( par_counter != 0 )
    {
        error_code = E_SYNTAX;
        return;
    }
    
    scanner_get_token( &token );
    
    if ( token.ttype == E_INVLD ) 
    {
        error_code = E_LEX;
        return;
    }
    
    if ( token.ttype == E_LABRACK)
    {
        nonterminal = ST_LIST;
        error_code = PDAStackPush( E_FUNCTION );
        #ifdef TESTY
            printf("FUNCTION WITHOUT PARAMS - right syntax\n");
        #endif
    }
    else
    {
        error_code = E_SYNTAX;
    }
}
    
extern inline E_ERROR_TYPE PDAStackInit ( void )
{
    if ( ( PDAStack.data = malloc( sizeof( TOKEN_TYPE ) * SIZEOF_PDASTACK ) ) == NULL )
	{
	    return E_INTERPRET_ERROR;
	}
    
    PDAStack.data[PDAStack.top] = E_EOF;
	return E_OK;
}

extern inline TOKEN_TYPE PDAStackTop ( void )
{
    return PDAStack.data[PDAStack.top];
}

extern inline void PDAStackPop ( void )
{
    PDAStack.top--;
}

extern inline E_ERROR_TYPE PDAStackPush ( TOKEN_TYPE type )
{
    if ( ++PDAStack.top == PDAStack.size )
    {
        PDAStack.size += SIZEOF_PDASTACK;
        TOKEN_TYPE *help;
        if ( ( help = realloc( PDAStack.data, PDAStack.size * sizeof( TOKEN_TYPE ) ) ) == NULL )
        {
            return E_INTERPRET_ERROR;
        }
        PDAStack.data = help;
    }
    
    PDAStack.data[PDAStack.top] = type;

    return E_OK;
}

extern inline void PDAStackFree( void )
{
    free( PDAStack.data );
}
    
/**
 * Hlavna funkcia, ktora riadi syntakticku analyzu
 * @param void
 * @return E_ERROR_TYPE (lexikalne, syntakticke a semanticke chyby + chyba alokacie)
 */
E_ERROR_TYPE check_syntax ( void )
{
    precedenceInit( );
    PDAStackInit( );
    nonterminal = ST_LIST;
    
    do {
        scanner_get_token( &token );
        if ( LLtab[nonterminal][token.ttype] == NULL)
        {
            return E_SYNTAX;
        }
        else
        {
            LLtab[nonterminal][token.ttype]( );
            if ( error_code == E_OK )
            {
                if ( token.ttype == E_EOF )
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    } while ( 1 ); 
    
    precedenceShutDown( );
    PDAStackFree( );
    return error_code;
}

#else

static T_token token;
static E_ERROR_TYPE error_code;

/** deklaracie funkcii */
bool st_list( void );
bool st_list2( void );
bool par( void );
bool par( void );
bool par_list( void );
bool st_else( void );
bool while_end( void );
bool if_end( void );
bool func_end( void );
E_ERROR_TYPE check_syntax ( void );

#ifdef TESTY
#define PRINT_DEBUG(x) do { printf(x); } while(0)
#else
#define PRINT_DEBUG(x) do {} while (0)
#endif

bool st_list( void )
{
    scanner_get_token( &token );
    
    switch ( token.ttype ) 
    {
        case E_WHILE:
            PRINT_DEBUG("ST_LIST: dosiel mi while\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                #ifndef TESTY
                    error_code = setstate( S_WHILE_BEGIN );
                    if ( error_code != E_OK )
                        return false;
                #endif
                error_code = evaluate_expr( &token, E_RPARENTHESES );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
                return st_list2() && while_end() && st_list();
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            break;
            
        case E_IF:
            PRINT_DEBUG("ST_LIST: dosiel mi if\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {   
                #ifndef TESTY
                    error_code = setstate( S_IF_BEGIN );
                    if ( error_code != E_OK )
                        return false;
                #endif
                error_code = evaluate_expr( &token, E_RPARENTHESES );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                return st_list2() && st_else() && st_list2() && if_end() && st_list();
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            break;
        case E_FUNCTION:
            PRINT_DEBUG("ST_LIST: dosla mi funkcia\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_IDENT )
            {
                #ifndef TESTY
                    error_code = define( &token );
                    if ( error_code != E_OK )
                        return false;
                #endif
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                if ( par() && par_list() )
                {
                    if ( token.ttype != E_RPARENTHESES )
                    {
                        if (token.ttype == E_INVLD)
                            error_code = E_LEX;
                        else 
                            error_code = E_SYNTAX;
                        return false;
                    }
                        
                    scanner_get_token( &token );
                    
                    if ( token.ttype == E_LABRACK )
                    {
                        #ifndef TESTY
                            error_code = addparam( NULL );
                            if ( error_code != E_OK )
                                return false;
                        #endif
                        return st_list2() && func_end() && st_list();
                    }
                    else
                    {
                        if (token.ttype == E_INVLD)
                            error_code = E_LEX;
                        else 
                            error_code = E_SYNTAX;
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            break;
            
        case E_RETURN:
            PRINT_DEBUG("ST_LIST: dosiel mi return\n");
            #ifndef TESTY
                error_code = assign( NULL );
                if ( error_code != E_OK )
                    return false;
            #endif
            scanner_get_token( &token );
            
            error_code = evaluate_expr( &token, E_SEMICL );
            if ( error_code != E_OK )
                return false;
            
            return st_list();
            
        case E_VAR:
            PRINT_DEBUG("ST_LIST: dosla mi var\n");
            #ifndef TESTY
                error_code = assign( &token );
                if ( error_code != E_OK )
                    return false;
            #endif
            scanner_get_token( &token );
            
            if ( token.ttype == E_EQ )
            {   
                scanner_get_token( &token );
                error_code = evaluate_expr( &token, E_SEMICL );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            return st_list();
            
        case E_EOF:
            #ifndef TESTY
                error_code = setstate( S_FILE_END );
                if ( error_code != E_OK )
                    return false;
            #endif
            return true;
        default:
            PRINT_DEBUG("ST_LIST: dosiel mi default\n");
            if (token.ttype == E_INVLD)
                error_code = E_LEX;
            else 
                error_code = E_SYNTAX;
            return false;
    }
    
    return true;
}

bool st_list2( void )
{
    scanner_get_token( &token );
    
    switch ( token.ttype ) 
    {
        case E_WHILE:
            PRINT_DEBUG("ST_LIST2: dosiel mi while\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                #ifndef TESTY
                    error_code = setstate( S_WHILE_BEGIN );
                    if ( error_code != E_OK )
                        return false;
                #endif
                error_code = evaluate_expr( &token, E_RPARENTHESES );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
                return st_list2() && while_end() && st_list2();
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            break;
            
        case E_IF:
            PRINT_DEBUG("ST_LIST2: dosiel mi if\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                #ifndef TESTY
                    error_code = setstate( S_IF_BEGIN );
                    if ( error_code != E_OK )
                        return false;
                #endif
                error_code = evaluate_expr( &token, E_RPARENTHESES );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
                return st_list2() && st_else() && st_list2() && if_end() && st_list2();
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            break;
            
        case E_RETURN:
            PRINT_DEBUG("ST_LIST2: dosiel mi return\n");
            #ifndef TESTY
                error_code = assign( NULL );
                if ( error_code != E_OK )
                    return false;
            #endif
            scanner_get_token( &token );
            
            error_code = evaluate_expr( &token, E_SEMICL );
            if ( error_code != E_OK )
                return false;
            
            return st_list2();
            
        case E_VAR:
            PRINT_DEBUG("ST_LIST2: dosla mi premenna\n");
            #ifndef TESTY
                error_code = assign( &token );
                if ( error_code != E_OK )
                    return false;
            #endif
            scanner_get_token( &token );
            
            if ( token.ttype == E_EQ )
            {   
                scanner_get_token( &token );
                error_code = evaluate_expr( &token, E_SEMICL );
                if ( error_code != E_OK )
                    return false;
            }
            else
            {
                if (token.ttype == E_INVLD)
                    error_code = E_LEX;
                else 
                    error_code = E_SYNTAX;
                return false;
            }
            
            return st_list2();
            
        case E_RABRACK:
            PRINT_DEBUG("ST_LIST2: dosla mi }\n");
            return true;
            
        default:
            PRINT_DEBUG("ST_LIST2: chybny token\n");
            if (token.ttype == E_INVLD)
                error_code = E_LEX;
            else 
                error_code = E_SYNTAX;
            return false;
    }
    
    return true;
}

bool par( void )
{
    scanner_get_token( &token );
    
    if ( token.ttype == E_VAR )
    {   
        PRINT_DEBUG("PAR: dosla mi premenna\n");
        #ifndef TESTY
            error_code = addparam( &token );
            if ( error_code != E_OK )
                return false;
        #endif
        scanner_get_token( &token );
        return true;
    }
    else if ( token.ttype == E_RPARENTHESES )
    {
        PRINT_DEBUG("PAR: epsilon\n");
        return true;
    }
    else
    {   
        PRINT_DEBUG("PAR: chybny token\n");
        if (token.ttype == E_INVLD)
            error_code = E_LEX;
        else 
            error_code = E_SYNTAX;
        return false;
    }
    return true;
}

bool par_list( void )
{
    if ( token.ttype == E_RPARENTHESES )
    {
        PRINT_DEBUG("PAR_LIST: epsilon\n");
        return true;
    }
    else if ( token.ttype == E_COMA )
    {
        PRINT_DEBUG("PAR_LIST: dosla mi ciarka\n");
        scanner_get_token( &token );
        if ( token.ttype == E_VAR )
        {
            #ifndef TESTY
                error_code = addparam( &token );
                if ( error_code != E_OK )
                    return false;
            #endif
            PRINT_DEBUG("PAR_LIST: dosla mi premenna\n");
            scanner_get_token( &token );
            return par_list();
        }
        else
        {
            PRINT_DEBUG("PAR_LIST: chybny token\n");
            if (token.ttype == E_INVLD)
                error_code = E_LEX;
            else 
                error_code = E_SYNTAX;
            return false;
        }
    }
    else
    {
        PRINT_DEBUG("PAR_LIST: chybny token\n");
        if (token.ttype == E_INVLD)
            error_code = E_LEX;
        else 
            error_code = E_SYNTAX;
        return false;
    }
    return true;
}

bool st_else( void )
{   
    if ( token.ttype == E_RABRACK )
    {
        PRINT_DEBUG("ST_ELSE: RABRACK\n");
        scanner_get_token( &token );
        if ( token.ttype == E_ELSE )
        {
            PRINT_DEBUG("ST_ELSE: E_ELSE\n");
            scanner_get_token( &token );
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("ST_ELSE: LABRACK\n");
                #ifndef TESTY
                    error_code = setstate( S_IF_ELSE );
                    if ( error_code != E_OK )
                        return false;
                #endif
                return true;
            }
        }
    }
    
    PRINT_DEBUG("ST_ELSE: chybny token\n");
    if (token.ttype == E_INVLD)
        error_code = E_LEX;
    else 
        error_code = E_SYNTAX;
    return false;
}

bool while_end( void )
{
    PRINT_DEBUG("WHILE_END\n");
    #ifndef TESTY
        error_code = setstate( S_WHILE_END );
        if ( error_code != E_OK )
            return false;
    #endif
    return true;
}

bool if_end( void )
{
    PRINT_DEBUG("IF_END\n");
    #ifndef TESTY
        error_code = setstate( S_IF_END );
        if ( error_code != E_OK )
            return false;
    #endif
    return true;
}

bool func_end( void )
{
    PRINT_DEBUG("FUNC_END\n");
    #ifndef TESTY
        error_code = setstate( S_FUNCTION_END );
        if ( error_code != E_OK )
            return false;
    #endif
    return true;
}

E_ERROR_TYPE check_syntax ( void )
{
    precedenceInit( );
    #ifndef TESTY
        error_code = GeneratorInit( );
        if ( error_code != E_OK )
            return false;
    #endif
    
    if (st_list())
        PRINT_DEBUG("je to panske\n");
    else
    {
        #ifndef TESTY
            GeneratorErrorCleanup( );
        #endif
    }
    
    //todo
    #ifndef TESTY
        GeneratorErrorCleanup( );
    #endif
    precedenceShutDown ( );
    return error_code;
}

#endif