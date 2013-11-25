/**
 * @file syntax.c
 *
 * @brief implementacia syntaktickej analyzy
 * @author Stevo
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "scanner.h"
#include "expressions.h"
#include "syntax.h"

#define TESTY

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
    [PAR_LIST][E_LESSEQ] = NULL, [PAR_LIST][E_GREATEREQ] = NULL, [PAR_LIST][E_RPARENTHESES] = NULL,
    [PAR_LIST][E_IDENT] = NULL, [PAR_LIST][E_LABRACK] = NULL, [PAR_LIST][E_SEMICL] = NULL,
    [PAR_LIST][E_INT] = NULL, [PAR_LIST][E_DOUBLE] = NULL, [PAR_LIST][E_LITER] = NULL,
    [PAR_LIST][E_WHILE] = NULL, [PAR_LIST][E_FUNCTION] = NULL, [PAR_LIST][E_IF] = NULL,
    [PAR_LIST][E_ELSE] = NULL, [PAR_LIST][E_RETURN] = NULL, [PAR_LIST][E_FALSE] = NULL,
    [PAR_LIST][E_NULL] = NULL, [PAR_LIST][E_TRUE] = NULL, [PAR_LIST][E_EQ] = NULL,
    [PAR_LIST][E_RABRACK] = NULL, [PAR_LIST][E_INVLD] = NULL, [PAR_LIST][E_EOF] = NULL,
    
    //priradenie obsluznych ukazatelov podla pravidiel
    [PAR_LIST][E_VAR] = par_ident_treat,
    
};

/** globalne premenne */
static enum NONTERM_TYPES nonterminal;
static T_token token;
static E_ERROR_TYPE error_code;

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
    //error_code = E_OK;
    
    do {
        scanner_get_token( &token );
        printf("typ tokenu je: %d\n", token.ttype);
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
                printf("error_code %d\n", error_code);
                break;
            }
        }
    } while ( 1 ); 
    
    precedenceShutDown( );
    PDAStackFree( );
    printf("error_code %d\n", error_code);
    return error_code;
}