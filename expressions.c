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

/** inicializacne velkosti zasobnikov, ktorych pamat sa pri naplneni linearne zvacsuje */
#define SIZEOF_ESTACK 64
#define SIZEOF_PFXSTACK 64

#define TESTY

#ifdef TESTY
#include <string.h>
#endif

/** struktura eStack zasobnik pre precedencnu analyzu (e - enum stack)*/
static struct {
	TOKEN_TYPE *data;
	int top;
	TOKEN_TYPE last_terminal;
	int topterm;
	int size;
} eStack;

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
extern inline void estackSet ( void );
extern inline TOKEN_TYPE estackTop ( void );
extern inline E_ERROR_TYPE estackPop ( void );
extern inline E_ERROR_TYPE estackPush ( TOKEN_TYPE type );
extern inline E_ERROR_TYPE estackChangeLT( void );
extern inline void copy_token ( T_token *t1, T_token *t2 );
extern inline void findterm( void );

/** precedencna tabulka */
/*
|  R_C  |  R_E  |  R_P  |  R_N  |
|   <   |   >   |   =   |   x   |
*/
const TOKEN_TYPE prec_table [][16] = {
/* XXXX      .    !==   ===    +     *     -     /     <     >     <=    >=    (     )    TERM   {     ;      */
/*  .  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/* !== */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_E,  R_C,  R_E,  R_E},
/* === */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  +  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  *  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  -  */  {R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  /  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  <  */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  >  */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  <= */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  >= */  {R_C,  R_E,  R_E,  R_C,  R_C,  R_C,  R_C,  R_E,  R_E,  R_E,  R_E,  R_C,  R_E,  R_C,  R_E,  R_E},
/*  (  */  {R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_P,  R_C,  R_N,  R_N},
/*  )  */  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_N,  R_E,  R_N,  R_E,  R_E},
/* TERM*/  {R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_E,  R_N,  R_E,  R_N,  R_E,  R_E},
/*  {  */  {R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_C,  R_N,  R_C,  R_N,  R_N},
};

#ifdef TESTY
const char *enums[] = { //iba na testovacie ucely
".", "!==", "===","+","*","-","/","<",">","<=",">=","(",")","i","{",";","E_VAR","E_INT","E_DOUBLE",    
"E_LITER","EVAL", "SHIFT", "x","PUSH","E","E_EQ","E_RABRACK","E_COMA","E_IDENT","E_INVLD","E_WHILE",
 "E_FUNCTION","E_FALSE","E_NULL","E_TRUE","E_IF","E_ELSE","E_RETURN","E_MALLOC","E_EOF"
};

E_ERROR_TYPE eval(T_token *op1, T_token *op2, TOKEN_TYPE operation)
{
    static int counter = 0;
    printf("\x1b[31mINSTRUCTION NUMBER %d:\x1b[0m    ", counter);
    if (op1->ttype == E_VAR)
    {
        for (unsigned i = 0; i < op1->length; putchar(op1->data._string[i]), i++); putchar(' ');
    }
    else 
    {
        switch (op1->ttype) { 
            case E_FALSE: printf("FALSE ");  break; 
            case E_TRUE: printf("TRUE "); break; 
            case E_NULL: printf("NULL "); break;
            case E_INT: printf("%d ", op1->data._int); break;
            case E_DOUBLE: printf("%e ", op1->data._double); break;
            case E_LITER: printf("\"%s\" ", op1->data._string); break;
            case E_E: printf("L%d ", op1->data._int); break;
            default: break;
        }
    }
    printf("\x1b[32m%s\x1b[0m ", enums[operation]);
    if (op2->ttype == E_VAR)
    {
        for (unsigned i = 0; i < op2->length; putchar(op2->data._string[i]), i++); putchar('\n');
    }
    else 
    {
        switch (op2->ttype) { 
            case E_FALSE: printf("FALSE\n");  break; 
            case E_TRUE: printf("TRUE\n"); break; 
            case E_NULL: printf("NULL\n"); break;
            case E_INT: printf("%d\n", op2->data._int); break;
            case E_DOUBLE: printf("%e\n", op2->data._double); break;
            case E_LITER: printf("\"%s\"\n", op2->data._string); break;
            case E_E: printf("L%d\n", op2->data._int); break;
            default: break;
        }
    }
    op1->ttype = E_E;
    op1->data._int = counter;
    counter++;
    free(op2);
    return E_OK;
}
#endif

/**
 * Funkcia alokuje pamat zasobnika PFXStack
 *
 * @param void
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE PFXInit ( void )
{
	if ( ( PFXStack.postfix = malloc( sizeof( T_token * ) * SIZEOF_PFXSTACK ) ) == NULL )
	{
	    return E_INTERPRET_ERROR;
	}
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
        if ( ( help = realloc( PFXStack.postfix, PFXStack.max_size ) ) == NULL )
        {
            return E_INTERPRET_ERROR;
        }
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
	{
	    return E_INTERPRET_ERROR;
	}

	eStack.data[0] = E_LABRACK;
	eStack.top = 0;
	eStack.topterm = 0;
	eStack.last_terminal = E_LABRACK;
	eStack.size = SIZEOF_ESTACK;
    
	return E_OK;
}

/**
 * Funkcia nastavi strukturu eStack na rovnake hodnoty ako pri inicializacii ale jej velkost ponecha bezo zmeny
 *
 * @param void
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline void estackSet ( void )
{
    eStack.top = 0;
	eStack.topterm = 0;
	eStack.last_terminal = E_LABRACK;
	eStack.size = SIZEOF_ESTACK;
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
 * Funkcia vracia prvy terminal z vrcholu zasobnika
 * invariant - skor ako by na vrchole nebol terminal doslo by k chybe mimo tejto funkcie
 * preto nema zmysel kontrolovat ci na vrchole nejaky je
 * @param void
 * @return ukazatel na strukturu T_token
 */
extern inline TOKEN_TYPE estackTop ( void )
{
	return eStack.last_terminal;
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
        if ( eStack.data[eStack.top--] == R_C )
        {
            findterm( );
            eStack.data[++eStack.top] = E_E;
            return E_OK;
        }
    }
    else if ( help == E_RPARENTHESES )
    {
        if ( eStack.data[eStack.top--] == E_E )
        {
            if ( eStack.data[eStack.top--] == E_LPARENTHESES )
            {
                if ( eStack.data[eStack.top--] == R_C )
                {
                    findterm( );
                    eStack.data[++eStack.top] = E_E;
                    return E_OK;
                }
                else
                {
                    return E_SYNTAX;
                }
            }
            else
            {
                return E_SYNTAX;
            }
        }
        else
        {
            return E_SYNTAX;
        }
    }
    else if ( help == E_E )
    {
        if ( ( help = eStack.data[eStack.top--] ) < E_LPARENTHESES )
        {
            if ( eStack.data[eStack.top--] == E_E )
            {
                if ( eStack.data[eStack.top--] == R_C )
                {
                    findterm( );
                    eStack.data[++eStack.top] = E_E;
                    //pri chybe je navratovou hodnotou predana semanticka chyba, vyuziva sa tu vyhodnocovanie parametrov zprava dolava (_cdecl)
                    return eval( PFXStackTop( ), PFXStackTopPop( ), help ); 
                }
                else
                {
                    return E_SYNTAX;
                }
            }
            else
            {
                return E_SYNTAX;
            }
        }
        else
        {
            return E_SYNTAX;
        }
    }
    else
    {
        return E_SYNTAX;
    }

    return E_SYNTAX;    //len kvoli kompilatoru
}

/**
 * Funkcia ulozi prvok do zasobnika a nastavi topterm 
 * @param TOKEN_TYPE
 * @return chybovy kod (uspesna resp. neuspesna alokacia)
 */
extern inline E_ERROR_TYPE estackPush ( TOKEN_TYPE type )
{
    if (++eStack.top == eStack.size)
    {
        eStack.size = eStack.size + SIZEOF_ESTACK;
        TOKEN_TYPE *help;
        if ((help = realloc(eStack.data, eStack.size)) == NULL)
        {
            return E_INTERPRET_ERROR;
        }
        eStack.data = help;
    }

    eStack.data[eStack.top] = type;

    eStack.topterm = eStack.top;
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
    if ( i == eStack.size )
    {
        eStack.size = eStack.size + SIZEOF_ESTACK;
        TOKEN_TYPE *help;
        if ( ( help = realloc( eStack.data, eStack.size ) ) == NULL )
        {
            return E_INTERPRET_ERROR;
        }
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
 * Funkcia vyhlada na zasobniku terminal najblizsie vrcholu
 * @param void
 * @return void
 */
extern inline void findterm ( void )
{
    int i = eStack.top;
    if ( eStack.data[i] == E_E ) i--;
    eStack.last_terminal = eStack.data[i];
    eStack.topterm = i;
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
 * Hlavna funkcia, ktora riadi vyhodnocovanie vyrazov
 * @param void
 * @return E_ERROR_TYPE (lexikalne, syntakticke a semanticke chyby + chyba alokacie)
 */
E_ERROR_TYPE evaluate_expr ( T_token * start_token, TOKEN_TYPE termination_ttype )
{
    E_ERROR_TYPE error_code;
    estackSet( );
    
    TOKEN_TYPE actual_ttype;    //premenna pre uchovavanie aktualneho typu struktury token
    T_token * token;
    
    if ( ( token = malloc( sizeof( T_token ) ) ) == NULL )
    {
        return E_INTERPRET_ERROR;
    }
    
    copy_token( token, start_token );   //aby nevznikali konflikty pri uvolnovani pamate
    
    if ( termination_ttype == E_RPARENTHESES )  //token obsahuje terminal lavu zatvorku, ktora oznacuje zaciatok vyrazu v podmienke
    {
        scanner_get_token( token ); //mozem to tu testovat na prazdny vyraz ak by som chcel vypisat chybu
    }

    if ( ( actual_ttype = token->ttype ) > E_SEMICL )
    {
        if ( actual_ttype <= E_LITER )
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
                free( token );
                return E_LEX;
            }
            free( token );
            return E_SYNTAX; //chybny vstupny token
        }
    }
    
    do {
        #ifdef TESTY
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
                    PFXdispose( ); if ( actual_ttype != E_TERM ) free( token );
                    return error_code;   //najskor neexistuje pravidlo v gramatike
                }
                break;

            case R_C:
                if ( estackChangeLT( ) != E_OK )
                {
                    PFXdispose( ); if ( actual_ttype != E_TERM ) free( token );
                    return E_INTERPRET_ERROR;
                }
                if ( estackPush( actual_ttype ) != E_OK )
                {
                    PFXdispose( ); if ( actual_ttype != E_TERM ) free( token );
                    return E_INTERPRET_ERROR;
                }
                
                scanner_get_token( token );
                
                if ( ( actual_ttype = token->ttype ) > E_SEMICL )
                {
                    if ( actual_ttype <= E_LITER )
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
                            PFXdispose( ); free( token );
                            return E_LEX;
                        }
                        PFXdispose( ); free( token );
                        return E_SYNTAX; //chybny vstupny token
                    }
                }
                break;

            case R_P:
                if ( estackPush( actual_ttype ) != E_OK )
                {
                    PFXdispose( ); if ( actual_ttype != E_TERM ) free( token );
                    return E_INTERPRET_ERROR;
                }
                
                scanner_get_token( token );
                
                if ( ( actual_ttype = token->ttype ) > E_SEMICL )
                {
                    if ( actual_ttype <= E_LITER )
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
                            PFXdispose( ); free( token );
                            return E_LEX;
                        }
                        PFXdispose( ); free( token );
                        return E_SYNTAX; //chybny vstupny token
                    }
                }
                break;
            default:
                PFXdispose( ); if ( actual_ttype != E_TERM ) free( token );
                return E_SYNTAX;  //chyba neexistuje pravidlo v tabulke
        }    
    } while ( ( eStack.last_terminal != E_LABRACK ) || ( actual_ttype != termination_ttype ) );
    #ifdef TESTY
        printf("Ukoncil sa cyklus a na vstupe je: \x1B[31m%s\x1B[0m\n", enums[actual_ttype]);
        for (int i = 0; i <= eStack.top; i++) { printf("\x1B[32m------\x1B[0m"); }
        printf("\n");
        for (int i = 0; i < eStack.top; i++) { printf(" %s  \x1B[32m|\x1B[0m ", enums[eStack.data[i]]);}
        printf(" %s  \x1B[32m|\x1B[0m\n", enums[eStack.data[eStack.top]]);
        for (int i = 0; i <= eStack.top; i++) { printf("\x1B[32m------\x1B[0m"); }
        printf("\n");
    #endif
    
    printf("uspech\n");
    //tu sa este doplni volanie semantickeho ze vyraz sa vyhodnotil
    free( token );
    PFXdispose( );
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
    {
        return E_INTERPRET_ERROR;
    }
    
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