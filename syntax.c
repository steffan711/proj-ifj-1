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