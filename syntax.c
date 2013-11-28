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
#include "runtime.h"
#include "debug.h"

//#define TESTY

/** globalne premenne */
static T_token token;
static E_ERROR_TYPE error_code;
static bool termination_control = false;

/** deklaracie funkcii */
bool st_list( void );
bool st_list2( void );
bool st_list3( void );
bool par( void );
bool par_list( void );
E_ERROR_TYPE check_syntax ( void );

/** pomocne testovacie makra */
#ifdef TESTY
#define PRINT_DEBUG(...) do { printf(__VA_ARGS__); } while(0)
#else
#define PRINT_DEBUG(x) do {} while (0)
#endif

/** makra na skratenie a sprehladnenie kodu */
//kontrola pri chybe ci ide o syntakticku alebo lexikalnu
#define ERROR_CHECK(...) \
do {\
    termination_control = true;\
    if (token.ttype == E_INVLD)\
    {\
        fprintf( stderr, "Error on line %u: lexical error\n", token.line );\
        error_code = E_LEX;\
    }\
    else\
    {\
        fprintf(stderr, __VA_ARGS__ );\
        error_code = E_SYNTAX;\
    }\
    return false;\
} while (0)

//kontrola ci nenastala chyba pri volani funkcie z ineho modulu
#define ERRORCODE_CHECK(x) do {\
    error_code = x;\
    if ( error_code != E_OK )\
    {\
        termination_control = true;\
        return false;\
    }\
} while (0)

/**
 * Implementacia pravidiel LL gramatiky, ktore maju na lavej strane neterminal <st_list>
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
bool st_list( void )
{
    switch ( token.ttype ) 
    {
        case E_WHILE:
            PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi while\x1b[0m\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi zaciatok podmienky while '('\x1b[0m\n");
                #ifndef TESTY
                    ERRORCODE_CHECK( setstate( S_WHILE_BEGIN ) );
                #endif
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_RPARENTHESES ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of condition got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi zaciatok cyklu while '{'\x1b[0m\n");
                
                scanner_get_token( &token );
                
                if ( st_list2() )
                {
                    PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi koniec cyklu while '}'\x1b[0m\n");
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_WHILE_END ) );
                    #endif
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '{' at the beginning of while cycle got %s\n" , token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        case E_IF:
            PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi if\x1b[0m\n");
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            { 
                PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi zaciatok podmienky if '('\x1b[0m\n");
                #ifndef TESTY
                    ERRORCODE_CHECK( setstate( S_IF_BEGIN ) );
                #endif
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_RPARENTHESES ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of condition got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi zaciatok prikazu if '{'\x1b[0m\n");
                scanner_get_token( &token );
                
                if ( st_list2() )
                {
                    PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi koniec prvej casti if '}'\x1b[0m\n");
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_IF_ELSE ) );
                    #endif
                    return st_list3(); 
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '{' at the beginning of if statement got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        case E_FUNCTION:
            PRINT_DEBUG("\x1b[36mST_LIST: dosla mi funkcia\x1b[0m\n");
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_IDENT )
            {
                #ifndef TESTY
                    ERRORCODE_CHECK( define( &token ) );
                #endif
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting identifier of function got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                PRINT_DEBUG("\x1b[36mST_LIST: dosla mi lava zatvorka za nazvom funkcie\x1b[0m\n");
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of function declaration got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
                
            if ( par() && par_list() )
            {
                if ( token.ttype == E_LABRACK )
                {
                    PRINT_DEBUG("\x1b[36mST_LIST: dosla mi zatvorka '{' - zaciatok tela funkcie\x1b[0m\n");
                    #ifndef TESTY
                        ERRORCODE_CHECK( addparam( NULL ) );
                    #endif
                    scanner_get_token( &token );
                    
                    if ( st_list2() )
                    {
                        PRINT_DEBUG("\x1b[36mST_LIST: dosla mi ukoncovacia zatovorka definicie funkcie '}'\x1b[0m\n");
                        #ifndef TESTY
                            ERRORCODE_CHECK( setstate( S_FUNCTION_END ) );
                        #endif
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    ERROR_CHECK( "Error on line %u: expecting '{' as the beginning of function body got %s\n", token.line, TOKEN_NAME[token.ttype] );
                }
            }
            else
            {
                return false;
            }
            
            break;
            
        case E_RETURN:
            PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi return\x1b[0m\n");
            
            #ifndef TESTY
                ERRORCODE_CHECK( assign( NULL ) );
            #endif
            
            scanner_get_token( &token );
            ERRORCODE_CHECK( evaluate_expr( &token, E_SEMICL ) );
            scanner_get_token( &token );
            
            return true;
            
        case E_VAR:
            PRINT_DEBUG("\x1b[36mST_LIST: dosla mi var\x1b[0m\n");
            
            #ifndef TESTY
                ERRORCODE_CHECK( assign( &token ) );
            #endif
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_EQ )
            {   
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_SEMICL ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '=' after variable got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            return true;
            
        case E_EOF:
            #ifndef TESTY
                ERRORCODE_CHECK( setstate( S_FILE_END ) );
            #endif
            
            termination_control = true;
            return true;
            
        default:
            PRINT_DEBUG("\x1b[36mST_LIST: dosiel mi default\x1b[0m\n");
            ERROR_CHECK( "Error on line %u: expecting beginning of valid statement got %s\n", token.line, TOKEN_NAME[token.ttype] );
    }
    
    return true;
}

/**
 * Implementacia pravidiel LL gramatiky, ktore maju na lavej strane neterminal <st_list2>
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
bool st_list2( void )
{   
    switch ( token.ttype ) 
    {
        case E_WHILE:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi while\x1b[0m\n");
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi zaciatok podmienky while '('\x1b[0m\n");
                #ifndef TESTY
                    ERRORCODE_CHECK( setstate( S_WHILE_BEGIN ) );    
                #endif
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_RPARENTHESES ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of condition got %s\n" , token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi zaciatok cyklu while '{'\x1b[0m\n");
                
                scanner_get_token( &token );
                if ( st_list2() )
                {
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_WHILE_END ) );
                    #endif
                    return st_list2();
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '{' at the beginning of while cycle got %s\n" , token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        case E_IF:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi if\x1b[0m\n");
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi zaciatok podmienky if '('\x1b[0m\n");
                #ifndef TESTY
                    ERRORCODE_CHECK( setstate( S_IF_BEGIN ) );
                #endif
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_RPARENTHESES ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of condition got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi zaciatok prikazu if '{'\x1b[0m\n");
                
                scanner_get_token( &token );
                
                if ( st_list2() )
                {
                    PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi koniec prvej casti if '}'\x1b[0m\n");
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_IF_ELSE ) );
                    #endif
                    return st_list3() && st_list2(); 
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '{' at the beginning of if statement got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        case E_RETURN:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi return\x1b[0m\n");
            
            #ifndef TESTY
                ERRORCODE_CHECK( assign( NULL ) );
            #endif
            
            scanner_get_token( &token );
            ERRORCODE_CHECK( evaluate_expr( &token, E_SEMICL ) );
            scanner_get_token( &token );
            
            return st_list2();
            
        case E_VAR:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosla mi var\x1b[0m\n");
            
            #ifndef TESTY
                ERRORCODE_CHECK( assign( &token ) );
            #endif
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_EQ )
            {   
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_SEMICL ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '=' after variable got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            return st_list2();
            
        case E_RABRACK:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosla mi }\x1b[0m\n");
            scanner_get_token( &token );
            return true;
            
        default:
            PRINT_DEBUG("\x1b[36mST_LIST2: dosiel mi default\x1b[0m\n");
            ERROR_CHECK( "Error on line %u: expecting beginning of valid statement or '}' got %s\n", token.line, TOKEN_NAME[token.ttype] );
    }
    
    return true;
}

/**
 * Implementacia pravidiel LL gramatiky, ktore maju na lavej strane neterminal <st_list3>
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
bool st_list3( void )
{
    switch ( token.ttype )
    {
        case E_ELSE:
            PRINT_DEBUG("\x1b[36mST_LIST3: dosiel mi else\x1b[0m\n");
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST3: dosla mi '{' za prikazom else\x1b[0m\n");
                scanner_get_token( &token );
                if ( st_list2() )
                {
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_IF_END ) );
                    #endif
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '}' after else but got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        case E_ELSEIF:
            PRINT_DEBUG("\x1b[36mST_LIST3: dosiel mi elseif\x1b[0m\n");
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LPARENTHESES )
            {
                PRINT_DEBUG("\x1b[36mST_LIST3: dosiel mi zaciatok podmienky elseif '('\x1b[0m\n");
                #ifndef TESTY
                    ERRORCODE_CHECK( setstate( S_IF_BEGIN ) );
                #endif
                scanner_get_token( &token );
                ERRORCODE_CHECK( evaluate_expr( &token, E_RPARENTHESES ) );
            }
            else
            {
                ERROR_CHECK( "Error on line %u: expecting '(' at the beginning of condition got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            
            scanner_get_token( &token );
            
            if ( token.ttype == E_LABRACK )
            {
                PRINT_DEBUG("\x1b[36mST_LIST3: dosiel mi zaciatok prikazu elseif '{'\x1b[0m\n");
                scanner_get_token( &token );
                
                if ( st_list2() )
                {
                    PRINT_DEBUG("\x1b[36mST_LIST3: dosiel mi koniec elseif '}'\x1b[0m\n");
                    #ifndef TESTY
                        ERRORCODE_CHECK( setstate( S_IF_ELSE ) );
                    #endif
                    if ( st_list3() )
                    {
                        #ifndef TESTY
                            ERRORCODE_CHECK( setstate( S_IF_END ) );
                        #endif
                        return true;
                    }
                    else
                    {
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
                ERROR_CHECK( "Error on line %u: expecting '{' at the beginning of elseif statement got %s\n", token.line, TOKEN_NAME[token.ttype] );
            }
            break;
            
        default:
            #ifndef TESTY
                ERRORCODE_CHECK( setstate( S_IF_END ) );
            #endif
            return true;
    }
}

/**
 * Implementacia pravidiel LL gramatiky, ktore maju na lavej strane neterminal <par>
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
bool par( void )
{
    if ( token.ttype == E_VAR )
    {   
        PRINT_DEBUG("\x1b[36mPAR: dosiel mi identifikator premennej\x1b[0m\n");
        #ifndef TESTY
            ERRORCODE_CHECK( addparam( &token ) );
        #endif
        scanner_get_token( &token );
        return true;
    }
    else if ( token.ttype == E_RPARENTHESES )
    {
        PRINT_DEBUG("\x1b[36mPAR: epsilon\x1b[0m\n");
        return true;
    }
    else
    {   
        PRINT_DEBUG("\x1b[36mPAR: chybny token\x1b[0m\n");
        ERROR_CHECK( "Error on line %u: expecting identifier of param or ')' got %s\n", token.line, TOKEN_NAME[token.ttype] );
    }
    return true;
}

/**
 * Implementacia pravidiel LL gramatiky, ktore maju na lavej strane neterminal <par_list>
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
bool par_list( void )
{
    if ( token.ttype == E_RPARENTHESES )
    {
        PRINT_DEBUG("\x1b[36mPAR_LIST: epsilon\x1b[0m\n");
        scanner_get_token( &token );
        return true;
    }
    else if ( token.ttype == E_COMA )
    {
        PRINT_DEBUG("\x1b[36mPAR_LIST: dosla mi ciarka\x1b[0m\n");
        scanner_get_token( &token );
        if ( token.ttype == E_VAR )
        {
            #ifndef TESTY
                ERRORCODE_CHECK( addparam( &token ) );
            #endif
            PRINT_DEBUG("\x1b[36mPAR_LIST: dosiel mi identifikator premennej\x1b[0m\n");
            scanner_get_token( &token );
            return par_list();
        }
        else
        {
            PRINT_DEBUG("\x1b[36mPAR_LIST: chybny token\x1b[0m\n");
            ERROR_CHECK( "Error on line %u: expecting identifier of param got %s\n", token.line, TOKEN_NAME[token.ttype] );
        }
    }
    else
    {
        PRINT_DEBUG("\x1b[36mPAR_LIST: chybny token\x1b[0m\n");
        ERROR_CHECK( "Error on line %u: expecting ',' or ')' got %s\n", token.line, TOKEN_NAME[token.ttype] );
    }
    return true;
}

/**
 * Hlavna funkcia pre kontrolu syntaxe, ktora zaroven riadi generator kodu a semanticky analyzator
 *
 * @param void
 * @return bool (spravna resp. nespravna syntax daneho pravidla)
 */
E_ERROR_TYPE check_syntax ( void )
{
    precedenceInit( );
    #ifndef TESTY
        error_code = GeneratorInit( );
        if ( error_code != E_OK )
            return error_code;
    #endif
    scanner_get_token( &token );
    while ( !termination_control ) st_list();
    if ( error_code == E_OK )
    {
        struct InstructionTapeBuffer *ptr;
        //PrintTape( FT.tape );
        GeneratorPrepareTape(&ptr);
        //printf("pocet pasiek %u\n", ptr->size);
        
        InterpretCode( ptr->array[0] );
        
        //PrintTape( ptr->array[0] );
        //printf("/////////////////////////\n");
        //PrintTape( ptr->array[1] );
        //InterpretCode( ptr->array[0] );
        GeneratorDeleteTapes(ptr);
        PRINT_DEBUG("\x1b[36mje to panske\x1b[0m\n");
    }
    else
    {
        //obsluha toho ze sa stala chyba
    }
    
    //todo
    #ifndef TESTY
        GeneratorErrorCleanup( );
    #endif
    precedenceShutDown ( );
    return error_code;
}