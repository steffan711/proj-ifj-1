/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file debug.c Ladiace funkcie
 * @author Vladimír Čillo, Oliver Nemček, Štefan Martiček, Filip Ilavský
 */


#include <stdio.h> // printf
#include "scanner.h"
#include <stdarg.h>
#include "debug.h"

const char const *TOKEN_NAME[] = 
{
 "E_CONCAT" ,      
 "E_NOT_EQ" ,      
 "E_TRIPLEEQ" ,    
 "E_PLUS" ,        
 "E_MULT" ,        
 "E_MINUS" ,       
 "E_DIV" ,         
 "E_LESS" ,        
 "E_GREATER" ,     
 "E_LESSEQ" ,      
 "E_GREATEREQ" , 
 "E_OR1",
 "E_AND1",
 "E_OR2",
 "E_AND2", 
 "E_LPARENTHESES" ,
 "E_COMA" ,        
 "E_RPARENTHESES" ,
 "E_IDENT",       
 "E_TERM" ,
 "E_UMINUS",
 "E_NEG", 
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

const char const *OPCODE_NAME[] = 
{
"DUMMY", "START", "CREATE", "MOV", "RET", "PUSH", "COND", "JMP",
 "CALL", "CALL_BUILTIN",
 "INC", "DEC",
 "PLUS","MINUS",
 "CONCAT", "EQUAL", "NONEQUAL", "DIV", "MUL", "LESS", "GREATER", "LESSEQ", "GREATEREQ" 
};

#ifdef TIME_CHECKER
int pocet_tikov_za_sekundu;
struct tms casova_struktura;
clock_t zaciatok_merania_tikov;
clock_t koniec_merania_tikov;
#endif

/**
 * Vypise strukturu token na stdout
 * @param token
 */
void print_token( T_token* token )
{
    
    printf( "\nToken -> length: %u", token->length);
    switch( token->ttype ) 
    {
        case E_AND1:        printf( "\nToken -> ttype: E_AND1\n" );         break;
        case E_AND2:        printf( "\nToken -> ttype: E_AND2\n" );         break;
        case E_BOOL:        printf( "\nToken -> ttype: E_BOOL\n" );         break;
        case E_COMA:        printf( "\nToken -> ttype: E_COMA\n" );         break;
        case E_CONCAT:      printf( "\nToken -> ttype: E_CONCAT\n" );       break;
        case E_DIV:         printf( "\nToken -> ttype: E_DIV\n" );          break;
        case E_DOUBLE:      printf( "\nToken -> ttype: E_DOUBLE\n" );       break;
        case E_ELSE:        printf( "\nToken -> ttype: E_ELSE\n" );         break;
        case E_EOF:         printf( "\nToken -> ttype: E_EOF\n" );          break;
        case E_EQ:          printf( "\nToken -> ttype: E_EQ\n" );           break;
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
        case E_NEG:         printf( "\nToken -> ttype: E_NEG\n" );          break;
        case E_NOT_EQ:      printf( "\nToken -> ttype: E_NOT_EQ\n" );       break;
        case E_NULL:        printf( "\nToken -> ttype: E_NULL\n" );         break;
        case E_OR1:         printf( "\nToken -> ttype: E_OR1\n" );          break;
        case E_OR2:         printf( "\nToken -> ttype: E_OR2\n" );          break;
        case E_PLUS:        printf( "\nToken -> ttype: E_PLUS\n" );         break;
        case E_RABRACK:     printf( "\nToken -> ttype: E_RABRACK\n" );      break;
        case E_RETURN:      printf( "\nToken -> ttype: E_RETURN\n" );       break; 
        case E_RPARENTHESES:printf( "\nToken -> ttype: E_RPARENTHESES\n" ); break;
        case E_SEMICL:      printf( "\nToken -> ttype: E_SEMICL\n" );       break;
        case E_TRIPLEEQ:    printf( "\nToken -> ttype: E_TRIPLEEQ\n" );     break;
        case E_VAR:         printf( "\nToken -> ttype: E_VAR\n" );          break;
        case E_WHILE:       printf( "\nToken -> ttype: E_WHILE\n" );        break;
        case E_LOCAL:       printf( "\nToken -> ttype: E_LOCAL\n" );        break;
        case E_ELSEIF:      printf( "\nToken -> ttype: E_ELSEIF\n" );       break;
        default:                                                            break;
    
    } // switch
    
    
    printf( "Token -> data: ");
    if( token->ttype == E_INT )
        printf( "%d", token->data._int );
        
    else if( token->ttype == E_DOUBLE )
        printf( "%e", token->data._double );
   
    else if( token->ttype == E_BOOL )
        printf("%s", token->data._bool ? "true" : "false");
   
    else if( token->ttype == E_LOCAL )
        printf( "LOCALVAR!!" );

    else if( token->data._string )
        for( unsigned i = 0; i < token->length; i++)
            putchar(token->data._string[i]); 
    else
        printf( "NO DATA" );
    printf( "\nToken -> line: %u\n", token->line );
}


void  ERROR ( const char * format, ... )
{
  va_list args;
  va_start( args, format );
  vfprintf( stderr, format, args );
  va_end( args );
}

void print_char( FILE *file, char *text, unsigned int size )
{
    for( unsigned int i = 0; i < size; i++ )
    {
        putc( text[i], file );
    }
}