/**
 * @file expressions.c
 *
 * @brief implementacia precedencnej syntaktickej analyzy
 * @author Stevo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "scanner.h"
#include "expressions.h"
#include "debug.h"
#include "generator.h"

/** inicializacne velkosti zasobnikov, ktorych pamat sa pri naplneni linearne zvacsuje */
#define SIZEOF_ESTACK 32
#define SIZEOF_PFXSTACK 32

/** struktura eStack zasobnik pre precedencnu analyzu (e - enum stack)*/
static struct {
	int top;
    int size;
    TOKEN_TYPE *data;
	TOKEN_TYPE last_terminal;
} eStack = { .top = 0, .size = SIZEOF_ESTACK, .data = NULL, .last_terminal = E_SEMICL};

/** struktura na uchovavanie ukazatelov pre semanticku analyzu (postfix - princip postfixu) */
static struct {  
    T_token **postfix;
    int size;
    int max_size;
} PFXStack = { .postfix = NULL, .size = -1, .max_size = SIZEOF_PFXSTACK };

/** uplne funkcne prototypy lokalnych funkcii */
extern inline E_ERROR_TYPE PFXInit ( void );
extern inline E_ERROR_TYPE PFXStackPush ( T_token *token );
extern inline T_token * PFXStackTopPop ( void );
extern inline T_token * PFXStackTop ( void );
extern inline void PFXdispose ( void );
extern inline void PFXfree ( void );
extern inline E_ERROR_TYPE estackInit ( void );
extern inline E_ERROR_TYPE estackPop ( void );
extern inline E_ERROR_TYPE estackPush ( TOKEN_TYPE type );
extern inline E_ERROR_TYPE estackChangeLT( void );
extern inline void copy_token ( T_token *t1, T_token *t2 );
extern inline void findterm( void );
extern inline E_ERROR_TYPE function_analyze ( void );

/** precedencna tabulka */
/*
|  R_C  |  R_E  |  R_P  |  R_N  |
|   <   |   >   |   =   |   x   |
*/

const TOKEN_TYPE prec_table [][18] = {
/* XXXX      .    !==   ===    +     *     -     /     <     >     <=    >=    (     ,     )     f    TERM  un(-)  ;  */
/*  .  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/* !== */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/* === */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  +  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  *  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  -  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_N,  R_E},
/*  /  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  <  */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  >  */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  <= */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  >= */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_E},
/*  (  */  {R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_P,  R_P,  R_C,  R_C,  R_C,  R_N},
/*  ,  */  {R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_P,  R_P,  R_C,  R_C,  R_C,  R_N},
/*  )  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_N,  R_E,  R_E,  R_N,  R_N,  R_N,  R_E},
/*  f  */  {R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N,  R_P,  R_N,  R_N,  R_N,  R_N,  R_N,  R_N},
/* TERM*/  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_N,  R_E,  R_E,  R_N,  R_N,  R_N,  R_E},
/*un(-)*/  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_E,  R_C,  R_C,  R_N,  R_E},
/*  ;  */  {R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_N,  R_N,  R_C,  R_C,  R_C,  R_N},
};

//#define TESTY2
//#include <string.h>
const char *enums[] = { //iba na testovacie ucely
"." ,      
 "!==" ,      
 "===" ,    
 "+" ,        
 "*" ,        
 "-" ,       
 "/" ,         
 "<" ,        
 ">" ,     
 "<=" ,      
 ">=" ,   
 "(" ,
 "," ,        
 ")" ,
 "E_IDENT",       
 "E_TERM" ,
 "un(-)", 
 "E_SEMICL" ,      
 "E_VAR" ,         
 "E_INT" ,         
 "E_DOUBLE" ,      
 "E_LITER" ,       
 "E_BOOL" ,
 "E_NULL" ,
 "E_WHILE" ,
 "E_FUNCTION" ,
 "E_IF" ,
 "E_ELSE" ,
 "E_RETURN" ,
 "E_EQ" ,          
 "E_LABRACK" ,     
 "E_RABRACK" ,     
 "E_INVLD" ,       
 "E_EOF" , // padla
 "E_LOCAL" ,
 "R_E" ,           
 "R_C" ,           
 "R_N" ,           
 "R_P" ,           
 "E_E" ,  
 "E_ELSEIF" 
};

/**
 * Funkcia alokuje pamat zasobnika PFXStack
 *
 * @param void
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE PFXInit ( void )
{
	if ( ( PFXStack.postfix = malloc( sizeof( T_token * ) * SIZEOF_PFXSTACK ) ) == NULL )
	    return E_INTERPRET_ERROR;
	return E_OK;
}

/**
 * Funkcia prida do zasobnika polozku
 *
 * @param ukazatel na strukturu T_token
 * @return chybovy kod (uspesna resp. neuspesna realokacia)
 */
extern inline E_ERROR_TYPE PFXStackPush ( T_token *token )
{
    if ( ++PFXStack.size == PFXStack.max_size )
    {
        PFXStack.max_size += SIZEOF_PFXSTACK;
        T_token **help;
        if ( ( help = realloc( PFXStack.postfix, PFXStack.max_size * sizeof( T_token ** ) ) ) == NULL )
            return E_INTERPRET_ERROR;
        PFXStack.postfix = help;
    }
    
    PFXStack.postfix[PFXStack.size] = token;

    return E_OK;
}

/**
 * Funkcia vrati hodnotu polozky na vrchole PFXStack a sucasne odstrani tuto polozku z vrcholu zasobnika
 * invariant - ak pred tymto volanim nedoslo k chybe zasobnik je isto neprazdny
 * @param void
 * @return ukazatel na strukturu T_token
 */
extern inline T_token * PFXStackTopPop ( void )
{
    return PFXStack.postfix[PFXStack.size--];
}

/**
 * Funkcia vrati hodnotu polozky na vrchole PFXStack
 * invariant - ak pred tymto volanim nedoslo k chybe zasobnik je isto neprazdny
 * @param void
 * @return ukazatel na strukturu T_token
 */
extern inline T_token * PFXStackTop ( void )
{
    return PFXStack.postfix[PFXStack.size];
}

/**
 * Funkcia uvolni vsetky polozky v zasobniku PFX a da ho do stavu po inicializacii
 * @param void
 * @return void
 */
extern inline void PFXdispose ( void )
{
    for ( int i = PFXStack.size; i >= 0; free( PFXStack.postfix[i] ), i-- );
    PFXStack.size = -1;
}

/**
 * Funkcia uvolni polozku postfix v strukture PFXStack
 * @param void
 * @return void
 */
extern inline void PFXfree ( void )
{
    free( PFXStack.postfix );
}

/**
 * Funkcia alokuje pamat pre polozku data struktury eStack a sucasne inicializuje hodnoty tejto struktury
 *
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE estackInit ( void )
{
	if ( ( eStack.data = malloc( sizeof( TOKEN_TYPE ) * SIZEOF_ESTACK ) ) == NULL )
	    return E_INTERPRET_ERROR;

	eStack.data[0] = E_SEMICL;
    
	return E_OK;
}

/**
 * Funkcia uvolni polozku data v strukture eStack
 *
 * @param void
 * @return void
 */
extern inline void estackFree ( void )
{
    free( eStack.data );
}

/**
 * Funkcia vyhodnocuje vyraz na vrchole zasobnika eStack, meni jeho obsah, vyhlada posledny terminal na vrchole + vola semanticku funkciu eval 
 * @param void
 * @return chybovy kod semanticke aj syntakticke chyby (volanie semantickej funkcie eval)
 */
extern inline E_ERROR_TYPE estackPop ( void )
{
    TOKEN_TYPE help;    //pomocna premenna pre uchovanie hodnoty z vrcholu zasobnika

    if ( ( help = eStack.data[eStack.top--] ) == E_TERM )
    {
        if ( eStack.data[eStack.top] == R_C )
        {
            findterm( );
            eStack.data[eStack.top] = E_E;
            return E_OK;
        }
    }
    else if ( help == E_RPARENTHESES )
    {
        if ( ( help = eStack.data[eStack.top--] ) == E_E )
        {
            if ( ( help = eStack.data[eStack.top--] ) == E_LPARENTHESES )
            {
                if ( eStack.data[eStack.top] == R_C )
                {
                    findterm( );
                    eStack.data[eStack.top] = E_E;
                    return E_OK;
                }
                else if ( eStack.data[eStack.top] == E_IDENT && eStack.data[--eStack.top] == R_C )   //doplnenie analyzy funkcie typu f(E)
                {
                    findterm( );
                    eStack.data[eStack.top] = E_E;
                    PFXStack.size -= 1;
                    return evalf( &(PFXStack.postfix[PFXStack.size]), 1 );
                }
                else 
                {
                    return E_SYNTAX;
                }
            }
            else if ( help == E_COMA )  //doplnenie analyzy funkcie f(E,...)
            {
                return function_analyze( ); //v tele vola evalf aj nastavuje co je potrebne
            }
        }
        else
        {
            if ( help == E_LPARENTHESES )   //doplnenie analyzy funkcie typu f()
            {
                if ( eStack.data[eStack.top--] == E_IDENT )
                {
                    if ( eStack.data[eStack.top] == R_C )
                    {
                        findterm( );
                        eStack.data[eStack.top] = E_E;
                        return evalf( &(PFXStack.postfix[PFXStack.size]), 0 );
                    }
                }
            }
        }
    }
    else if ( help == E_E ) /* vyhodnocuje sa diadicka operacia */
    {
        if ( ( help = eStack.data[eStack.top--] ) < E_LPARENTHESES )
        {
            if ( eStack.data[eStack.top--] == E_E )
            {
                if ( eStack.data[eStack.top] == R_C )
                {
                    findterm( );
                    eStack.data[eStack.top] = E_E;
                    //pri chybe je navratovou hodnotou predana semanticka chyba, vyuziva sa tu vyhodnocovanie parametrov zprava dolava (_cdecl)
                    return eval( PFXStackTop( ), PFXStackTopPop( ), help );
                }
            }
        }
        else if ( help == E_UMINUS )
        {
            findterm( );
            eStack.data[eStack.top] = E_E;
            return eval( PFXStackTop( ), PFXStackTopPop( ), E_MINUS );
        }
    }

    return E_SYNTAX;
}

/**
 * Funkcia ulozi prvok do zasobnika a nastavi last_terminal
 * @param TOKEN_TYPE
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE estackPush ( TOKEN_TYPE type )
{
    if (++eStack.top == eStack.size)
    {
        eStack.size = eStack.size + SIZEOF_ESTACK;
        TOKEN_TYPE *help;
        if ( ( help = realloc( eStack.data, eStack.size * sizeof( TOKEN_TYPE ) ) ) == NULL )
            return E_INTERPRET_ERROR;
        eStack.data = help;
    }
    
    eStack.data[eStack.top] = type;
    eStack.last_terminal = type;
    return E_OK;
}

/**
 * Funkcia zameni na zasobniku prvok za prvok < (pravidlo precedencnej analyzy)
 * @param void
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE estackChangeLT ( void )
{
    int i = eStack.top++;
    if ( eStack.top == eStack.size )
    {
        eStack.size = eStack.size + SIZEOF_ESTACK;
        TOKEN_TYPE *help;
        if ( ( help = realloc( eStack.data, eStack.size * sizeof( TOKEN_TYPE ) ) ) == NULL )
            return E_INTERPRET_ERROR;
        eStack.data = help;
    }
    
    if ( eStack.data[i] == E_E )
    {
        eStack.data[eStack.top] = E_E;
        eStack.data[i] = R_C;
    }
    else
    {
        eStack.data[eStack.top] = R_C;
    }
    
    return E_OK;
}

/**
 * Funkcia urci na zasobniku terminal najblizsie vrcholu
 * @param void
 * @return void
 */
extern inline void findterm ( void )
{
    eStack.last_terminal = eStack.data[eStack.top - 1];
}

/**
 * Funkcia skorpiruje obsah struktury token v druhom parametri do struktury v prvom parametri
 * @param T_token * cielova struktura
 * @param T_token * zdrojova struktura
 * @return void
 */
extern inline void copy_token ( T_token *t1, T_token *t2 )
{
    t1->ttype = t2->ttype;
    t1->line = t2->line;
    t1->length = t2->length;
    t1->data = t2->data;
}

/**
 * Funkcia spocita parametre volania funkcie a zavola evalf pre vyhodnotenie vyrazu
 * @param void
 * @return E_ERROR_TYPE (syntakticke chyby)
 */
extern inline E_ERROR_TYPE function_analyze ( void )
{
    int counter = 2;    // v cykle kontrolujem az druhy parameter
    TOKEN_TYPE help;
    while ( 1 )
    {
        if ( eStack.data[eStack.top--] == E_E )
        {
            if ( ( help = eStack.data[eStack.top--] ) == E_COMA )
            {
                counter++;
                continue;
            }
            else if ( help == E_LPARENTHESES ) //rovno nastavi zasobnik na tie spravne hodnoty
            {
                if ( eStack.data[eStack.top--] == E_IDENT)
                {
                    eStack.data[eStack.top] = E_E;
                    findterm( );
                    PFXStack.size -= counter;
                    return evalf( &(PFXStack.postfix[PFXStack.size]), counter );
                }
            }
        }
        return E_SYNTAX;
    }
    
    return E_SYNTAX; //kvoli kompilatoru
}

/**
 * Hlavna funkcia, ktora riadi vyhodnocovanie vyrazov
 * @param void
 * @return E_ERROR_TYPE (lexikalne, syntakticke a semanticke chyby + chyba alokacie)
 */
E_ERROR_TYPE evaluate_expr ( T_token * start_token, TOKEN_TYPE termination_ttype )
{
    E_ERROR_TYPE error_code;
    
    TOKEN_TYPE actual_ttype;    //premenna pre uchovavanie aktualneho typu struktury token
    T_token * token;
    
    if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
        return E_INTERPRET_ERROR;
    
    copy_token( token, start_token ); //aby nevznikali konflikty pri uvolnovani pamate

    if ( ( actual_ttype = token->ttype ) > E_SEMICL )
    {
        if ( actual_ttype <= E_NULL )
        {
            actual_ttype = E_TERM;
            if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
            {
                free( token );
                return E_INTERPRET_ERROR;
            }
            //ak som nasiel nejaky term potom sa musi vyhradit nove miesto pre dalsi
            if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
            {
                PFXdispose();
                return E_INTERPRET_ERROR;
            }
        }
        else
        {
            if ( actual_ttype == E_INVLD )
            {
                fprintf( stderr, "Error on line %u: lexical error\n", token->line );
                free( token );
                return E_LEX;
            }
            free( token );
            fprintf( stderr, "Error near line %u: this expression must be terminate with '%c' got %s\n",\
            token->line, termination_ttype, TOKEN_NAME[token->ttype] );
            return E_SYNTAX; //chybny vstupny token
        }
    }
    else if ( actual_ttype == E_IDENT )
    {
        if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
        {
            free( token );
            return E_INTERPRET_ERROR;
        }
        //ak som nasiel nejaku funkciu potom sa musi vyhradit nove miesto pre dalsi
        if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
        {
            PFXdispose();
            return E_INTERPRET_ERROR;
        }
    }
    else if ( actual_ttype == E_MINUS )
    {
        if ( ! ( ( eStack.data[eStack.top] >= E_RPARENTHESES && eStack.data[eStack.top] <= E_UMINUS ) || eStack.data[eStack.top] == E_MINUS ) )
        {
            actual_ttype = E_UMINUS;
            //simulujem konstantu hodnoty 0
            token->ttype = E_INT;
            token->data._int = 0;
            if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
            {
                PFXdispose( ); free( token );
                return E_INTERPRET_ERROR;
            }
            if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
            {
                PFXdispose( );
                return E_INTERPRET_ERROR;
            }
        }
    }
    
    do {
        #ifdef TESTY2
            printf("na vstupe je: \x1B[32m%s\x1B[0m\n", enums[actual_ttype]);
            for (int i = 0; i <= eStack.top; i++) { printf("\x1B[34m-----\x1B[0m"); for(unsigned j = 0; j < strlen(enums[eStack.data[i]]); j++) printf("\x1B[34m-\x1B[0m");}
            printf("\n");
            for (int i = 0; i < eStack.top; i++) { printf(" %s  \x1B[34m|\x1B[0m ", enums[eStack.data[i]]);}
            printf(" %s  \x1B[34m|\x1B[0m\n", enums[eStack.data[eStack.top]]);
            for (int i = 0; i <= eStack.top; i++) { printf("\x1B[34m-----\x1B[0m"); for(unsigned j = 0; j < strlen(enums[eStack.data[i]]); j++) printf("\x1B[34m-\x1B[0m");}
            printf("\n");
        #endif
        switch ( prec_table[eStack.last_terminal][actual_ttype] )    //invariant - nikdy nepristupim na index mimo pola tabulky
        {
            case R_E:
                if ( ( error_code = estackPop( ) ) != E_OK )
                {
                    if ( error_code == E_SYNTAX )
                    {
                        if ( actual_ttype == E_TERM || actual_ttype == E_IDENT )
                        {
                            fprintf( stderr, "Error near line %u: bad syntax of expression\n", PFXStackTop()->line );
                        }
                        else
                        {
                            fprintf( stderr, "Error near line %u: bad syntax of expression\n", token->line );
                        }
                    }
                    PFXdispose( ); free( token );
                    return error_code;   //najskor neexistuje pravidlo v gramatike
                }
                break;

            case R_C:
                if ( estackChangeLT( ) != E_OK )
                {
                    PFXdispose( ); free( token );
                    return E_INTERPRET_ERROR;
                }
                if ( estackPush( actual_ttype ) != E_OK )
                {
                    PFXdispose( ); free( token );
                    return E_INTERPRET_ERROR;
                }
                
                scanner_get_token( token );
                
                if ( ( actual_ttype = token->ttype ) > E_SEMICL )
                {
                    if ( actual_ttype <= E_NULL )
                    {
                        actual_ttype = E_TERM;
                        if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                        {
                            PFXdispose( ); free( token );
                            return E_INTERPRET_ERROR;
                        }
                        //ak som nasiel nejaky term potom sa musi vyhradit nove miesto pre dalsi
                        if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                        {
                            PFXdispose();
                            return E_INTERPRET_ERROR;
                        }
                    }
                    else
                    {
                        if ( actual_ttype == E_INVLD )
                        {
                            fprintf( stderr, "Error on line %u: lexical error\n", token->line );
                            PFXdispose( ); free( token );
                            return E_LEX;
                        }
                        fprintf( stderr, "Error near line %u: this expression must be terminate with '%c' got %s\n",\
                        token->line, termination_ttype == E_SEMICL ? ';' : ')', TOKEN_NAME[token->ttype] );
                        PFXdispose( ); free( token );
                        return E_SYNTAX; //chybny vstupny token
                    }
                }
                else if ( actual_ttype == E_IDENT )
                {
                    if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                    {
                        PFXdispose( ); free( token );
                        return E_INTERPRET_ERROR;
                    }
                    //ak som nasiel nejaku funkciu potom sa musi vyhradit nove miesto pre dalsi
                    if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                    {
                        PFXdispose( );
                        return E_INTERPRET_ERROR;
                    }
                }
                else if ( actual_ttype == E_MINUS )
                {
                    if ( ! ( ( eStack.data[eStack.top] >= E_RPARENTHESES && eStack.data[eStack.top] <= E_UMINUS ) || eStack.data[eStack.top] == E_MINUS ) )
                    {
                        actual_ttype = E_UMINUS;
                        //simulujem konstantu hodnoty 0
                        token->ttype = E_INT;
                        token->data._int = 0;
                        if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                        {
                            PFXdispose( ); free( token );
                            return E_INTERPRET_ERROR;
                        }
                        if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                        {
                            PFXdispose( );
                            return E_INTERPRET_ERROR;
                        }
                    }
                }
                break;

            case R_P:
                if ( estackPush( actual_ttype ) != E_OK )
                {
                    PFXdispose( ); free( token );
                    return E_INTERPRET_ERROR;
                }
                
                scanner_get_token( token );
                
                if ( ( actual_ttype = token->ttype ) > E_SEMICL )
                {
                    if ( actual_ttype <= E_NULL )
                    {
                        actual_ttype = E_TERM;
                        if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                        {
                            PFXdispose( ); free( token );
                            return E_INTERPRET_ERROR;
                        }
                        //ak som nasiel nejaky term potom sa musi vyhradit nove miesto pre dalsi
                        if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                        {
                            PFXdispose( );
                            return E_INTERPRET_ERROR;
                        }
                    }
                    else
                    {
                        if ( actual_ttype == E_INVLD )
                        {
                            fprintf( stderr, "Error on line %u: lexical error\n", token->line );
                            PFXdispose( ); free( token );
                            return E_LEX;
                        }
                        fprintf( stderr, "Error near line %u: this expression must be terminate with '%c' got %s\n",\
                        token->line, termination_ttype == E_SEMICL ? ';' : ')', TOKEN_NAME[token->ttype] );
                        
                        PFXdispose( ); free( token );
                        return E_SYNTAX; //chybny vstupny token
                    }
                }
                else if ( actual_ttype == E_IDENT )
                {
                    if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                    {
                        PFXdispose( ); free( token );
                        return E_INTERPRET_ERROR;
                    }
                    //ak som nasiel nejaku funkciu potom sa musi vyhradit nove miesto pre dalsi
                    if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                    {
                        PFXdispose( );
                        return E_INTERPRET_ERROR;
                    }
                }
                else if ( actual_ttype == E_MINUS )
                {
                    if ( ! ( ( eStack.data[eStack.top] >= E_RPARENTHESES && eStack.data[eStack.top] <= E_UMINUS ) || eStack.data[eStack.top] == E_MINUS ) )
                    {
                        actual_ttype = E_UMINUS;
                        //simulujem konstantu hodnoty 0
                        token->ttype = E_INT;
                        token->data._int = 0;
                        if ( PFXStackPush( token ) != E_OK ) //prida do postfixu term
                        {
                            PFXdispose( ); free( token );
                            return E_INTERPRET_ERROR;
                        }
                        if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
                        {
                            PFXdispose( );
                            return E_INTERPRET_ERROR;
                        }
                    }
                }
                break;
                
            default:
                if ( actual_ttype == E_TERM || actual_ttype == E_IDENT )
                {
                    fprintf( stderr, "Error near line %u: bad syntax of expression\n", PFXStackTop()->line );
                }
                else
                {
                    fprintf( stderr, "Error near line %u: bad syntax of expression\n", token->line );
                }
                PFXdispose( ); free(token); 
                return E_SYNTAX;  //chyba neexistuje pravidlo v tabulke
        }    
    } while ( ( eStack.last_terminal != E_SEMICL ) || ( actual_ttype != termination_ttype ) );
   
    #ifdef TESTY2
        printf("Ukoncil sa cyklus a na vstupe je: \x1B[31m%s\x1B[0m\n", enums[actual_ttype]);
        for (int i = 0; i <= eStack.top; i++) { printf("\x1B[32m------\x1B[0m"); }
        printf("\n");
        for (int i = 0; i < eStack.top; i++) { printf(" %s  \x1B[32m|\x1B[0m ", enums[eStack.data[i]]);}
        printf(" %s  \x1B[32m|\x1B[0m\n", enums[eStack.data[eStack.top]]);
        for (int i = 0; i <= eStack.top; i++) { printf("\x1B[32m------\x1B[0m"); }
        printf("\n");
    #endif
    //posledne volanie funkcie eval s s jedinou polozkou na vrchole zasobnika PFXStack, ktorou je vysledok vyhodnotenia vyrazu
    if ( ( error_code = eval( PFXStackTopPop( ), NULL, E_TERM ) ) != E_OK )
    {
        free( token );
        return error_code;
    }
    
    eStack.top = 0; //inicializacia pre dalsie volanie
    free( token );
    return E_OK;
}

/**
 * Funkcia pre inicializaciu globalnych premennych
 * @param void
 * @return E_ERROR_TYPE (chyba alokacie)
 */
E_ERROR_TYPE precedenceInit ( void )
{
    if ( estackInit( ) != E_OK )
        return E_INTERPRET_ERROR;
    
    if ( PFXInit( ) != E_OK )
    {
        estackFree();
        return E_INTERPRET_ERROR;
    }
    
    return E_OK;
}

/**
 * Funkcia, ktora sluzi pre uvolnenie alokovanej pamate globalnych premennych
 * @param void
 * @return void
 */
void precedenceShutDown ( void )
{
    estackFree();
    PFXfree();
}