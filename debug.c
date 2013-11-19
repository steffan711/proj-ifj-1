

#include <stdio.h> // printf
#include "scanner.h"

/**
 * Vypise strukturu token na stdout
 * @param token
 */
extern void print_token( T_token* token )
{
    
    printf( "\nToken -> length: %u", token->length);
    switch( token->ttype ) 
    {
        case E_COMA:        printf( "\nToken -> ttype: E_COMA\n" );         break;
        case E_CONCAT:      printf( "\nToken -> ttype: E_CONCAT\n" );       break;
        case E_DIV:         printf( "\nToken -> ttype: E_DIV\n" );          break;
        case E_DOUBLE:      printf( "\nToken -> ttype: E_DOUBLE\n" );       break;
        case E_ELSE:        printf( "\nToken -> ttype: E_ELSE\n" );         break;
        case E_EOF:         printf( "\nToken -> ttype: E_EOF\n" );          break;
        case E_EQ:          printf( "\nToken -> ttype: E_EQ\n" );           break;
        case E_FALSE:       printf( "\nToken -> ttype: E_FALSE\n" );        break;
        case E_FUNCTION:    printf( "\nToken -> ttype: E_FUNCTION\n" );     break;
        case E_GREATEREQ:   printf( "\nToken -> ttype: E_GREATEREQ\n" );    break;
        case E_GREATER:     printf( "\nToken -> ttype: E_GREATER\n" );      break;
        case E_IDENT:       printf( "\nToken -> ttype: E_IDENT\n" );        break;
        case E_IF:          printf( "\nToken -> ttype: E_IF\n" );           break;
        case E_INT:         printf( "\nToken -> ttype: E_INT\n" );          break;
        case E_INVLD:       printf( "\nToken -> ttype: E_INVLD\n" );        break;
        case E_LABRACK:     printf( "\nToken -> ttype: E_LABRACK\n" );      break;
        case E_LESSEQ:      printf( "\nToken -> ttype: E_LESSEQ\n" );       break;
        case E_LESS:        printf( "\nToken -> ttype: E_LESS\n" );         break;
        case E_LITER:       printf( "\nToken -> ttype: E_LITER\n" );        break;
        case E_LPARENTHESES:printf( "\nToken -> ttype: E_LPARENTHESES\n" ); break;
        case E_MINUS:       printf( "\nToken -> ttype: E_MINUS\n" );        break;
        case E_MULT:        printf( "\nToken -> ttype: E_MULT\n" );         break;
        case E_NOT_EQ:      printf( "\nToken -> ttype: E_NOT_EQ\n" );       break;
        case E_NULL:        printf( "\nToken -> ttype: E_NULL\n" );         break;
        case E_PLUS:        printf( "\nToken -> ttype: E_PLUS\n" );         break;
        case E_RABRACK:     printf( "\nToken -> ttype: E_RABRACK\n" );     break;
        case E_RETURN:      printf( "\nToken -> ttype: E_RETURN\n" );       break; 
        case E_RPARENTHESES:printf( "\nToken -> ttype: E_RPARENTHESES\n" ); break;
        case E_SEMICL:      printf( "\nToken -> ttype: E_SEMICL\n" );       break;
        case E_TRIPLEEQ:    printf( "\nToken -> ttype: E_TRIPLEEQ\n" );     break;
        case E_TRUE:        printf( "\nToken -> ttype: E_true\n" );         break;
        case E_VAR:         printf( "\nToken -> ttype: E_VAR\n" );          break;
        case E_WHILE:       printf( "\nToken -> ttype: E_WHILE\n" );        break;
        default:                                                            break;
    
    } // switch
    
    
    printf( "Token -> data: ");
    if( token->ttype == E_INT )
        printf( "%d", token->data._int );
        
    else if( token->ttype == E_DOUBLE )
        printf( "%e", token->data._double );

    else if( token->data._string )
        for( unsigned i = 0; i < token->length; i++)
            putchar(token->data._string[i]); 
    else
        printf( "NO DATA" );
    printf( "\nToken -> line: %u\n", token->line );
}