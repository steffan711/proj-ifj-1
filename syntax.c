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

/**
 * Hlavna funkcia, ktora riadi syntakticku analyzu
 * @param void
 * @return E_ERROR_TYPE (lexikalne, syntakticke a semanticke chyby + chyba alokacie)
 */
 
E_ERROR_TYPE check_syntax ( void )
{
    T_token *token;
    if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
    {
        return E_INTERPRET_ERROR;
    }
    
    precedenceInit();
    scanner_get_token( token );
    if ( token->ttype == E_LPARENTHESES)
    {
        evaluate_expr( token, E_RPARENTHESES );
    }
    else 
    {
        evaluate_expr( token, E_SEMICL );
    }
    
    // scanner_get_token( token );
    // if ( token->ttype == E_LPARENTHESES)
    // {
        // evaluate_expr( token, E_RPARENTHESES );
    // }
    // else 
    // {
        // evaluate_expr( token, E_SEMICL );
    // }
    free(token);
    //do_precedence();
    precedenceShutDown();
    return E_OK;
}