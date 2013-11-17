/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xnemcek, xilavsk, xmarti
 * Subor scanner.c Modul lexikalneho analyzatoru
 *
 * @brief Modul lexikalneho analyzatoru
 * @author Vladimír Čillo, xcillo00
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"



/**
 * Funkcia naplni strukturu tokenu potrebnymi datami
 *
 * @param 1 ukazatel na token
 * @param 2 token type
 */
static inline void set_token( T_token* token, TOKEN_TYPE type, unsigned dlzka, void* data )
{
    token->line = scanner_line;
    token->length = dlzka;
    token->ttype = type;
    token->data._string = data;
}

	
/**
 * Funkcia realizuje prevod hexadecimalneho cisla na integer
 */
static inline int hex2int( char a, char b)
{
    int c;
    if ( a >= '0' && a <= '9' )
        c = (a-'0')*16;
    else
        c = (a-'A'+10)*16;
    if ( b >= '0' && b <= '9' )
        c = c+(b-'0');
    else
        c = c+(b-'A'+10);
    return c;
}



/**
 * @param 1 nacitany oddelovac
 * @param 2 specifikacia mnoziny validnych oddelovacov
 * @return rozhodne ci je token spravne ukonceny
 */
static inline bool is_divider( char znak, int switcher )
{
    if( switcher == number_divider ) // co moze nasledovat za operandom ?
        switch (znak)  
        {
            case '/':
            case '+':
            case '-':
            case '=':
            case ';':
            case ',':
            case EOF: // EOF
            case ')':
            case '(':
            case '*':
                return true;
            default: return isspace( znak );
        }
    else // co moze nasledovat za operatorom ?             
        switch (znak)
        {
            case '$':
            case EOF:
            case ')':
            case '(':
            case '"': 
                return true;
            default: return ( isspace( znak ) || isalnum( znak ) ); // znak, cislo alebo medzera je ok
        }
}


/**
 * @brief inicializuje scanner pred jeho prvym pouzitim
 * @param ukazatel na subor
 * @return Uspesnost
*/
void scanner_init( char *file_start )
{
    current_pos = file_start;
    scanner_line = 1;
    file_origin = file_start;
}

/**
 * debugovacia funkcia, vypisuje token
 * @param token
 */
void print_token( T_token* token )
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


extern inline int sstrcmp( const char * str1, const char * str2, int str1_size, int str2_size )
{
    if ( str1_size > str2_size )
        return 1;
    else if ( str1_size < str2_size )
        return -1;

    int result;
    const char * offset = str1 + str1_size;

    do {
        result = ( unsigned ) *str1++ - ( unsigned ) *str2++;
    } while ( ( result == 0 ) && ( str1 != offset ) );

    return result;
}


static inline void is_keyword(const char* word, T_token* token, unsigned lex_length) // testuje string na klucove slovo
{
    switch(word[0]) // prvy znak nam vela napovie
    {
        case 'e':   if ( sstrcmp( word, "else", lex_length, 4 ) == 0 ) 
                    {
                        token->ttype = E_ELSE;
                        token->data._string = NULL;
                    }
                    return;
        case 'n':   if ( sstrcmp( word, "null", lex_length, 4 ) == 0 )
                    {
                        token->ttype = E_NULL; 
                        token->data._string = NULL;
                    }
                    return;
        case 't':   if ( sstrcmp( word, "true", lex_length, 4 ) == 0 ) 
                    {
                        token->ttype = E_TRUE;
                        token->data._string = NULL;
                    }
                    return;
        case 'i':   if ( sstrcmp( word, "if", lex_length, 2 ) == 0 )
                    {
                        token->ttype = E_IF;
                        token->data._string = NULL;
                    }
                    return;            
        case 'r':   if ( sstrcmp( word, "return", lex_length, 6 ) == 0 )
                    {
                        token->ttype = E_RETURN;
                        token->data._string = NULL;
                    }
                    return;
        case 'w':   if ( sstrcmp( word, "while", lex_length, 5 ) == 0 )
                    {
                        token->ttype = E_WHILE;
                        token->data._string = NULL;
                    } 
                    return;
        case 'f':   if ( sstrcmp( word, "false", lex_length, 5 ) == 0 )
                    {
                        token->ttype = E_FALSE; 
                        token->data._string = NULL;
                    }
                    else if ( sstrcmp( word, "function", lex_length, 8 ) == 0 )
                    {
                        token->ttype = E_FUNCTION;
                        token->data._string = NULL; 
                    }
                    return;                    
        default:    return;
    }
}


/**
 * Po zavolani obsahuje parameter token nasledujuci token
 *
 * @param ukazatel na token (return)
 */
void scanner_get_token( T_token* token )
{
    // inicializacia :
    FSM_STATE next_state = INIT; 
    unsigned lex_length = 0;
    int znak;

    do
    {    
        znak = getc( current_pos );
        switch( next_state )
        {
            case INIT:     
            {
                while( isspace( znak ) )    // white space skip
                {
                    if( znak == '\n' )
                        scanner_line++;
                    znak = getc( current_pos );
                }

                if( isalpha( znak ) || znak == '_' ) // A-Za-z_
                {
                    next_state = T_ID;
                    token->ttype = E_IDENT;
                    break;
                }
                else if( isdigit( znak ) ) // 0-9
                {
                    lex_length++;
                    token->ttype = E_INT;
                    next_state = T_INT;
                    break;
                }
                else
                {
                    switch( znak )
                    {        
                        case '/':
                                        next_state = T_FRACTION;
                                        break;
                        case '$':
                                        next_state = T_ID;
                                        token->ttype = E_VAR;
                                        lex_length--; // $ nie je sucastou mena premennej
                                        break;
                        case ';':
                                        set_token( token, E_SEMICL, lex_length, NULL);
                                        return;	                                          
                        case '=':
                                        next_state = T_ASS;
                                        break;
                        case '+':
                                        set_token( token, E_PLUS, lex_length, NULL);
                                        return;
                        case '-':
                                        set_token( token, E_MINUS, lex_length, NULL);
                                        return;
                        case '*':
                                        set_token( token, E_MULT, lex_length, NULL);
                                        return;
                        case '<':
                                        next_state = T_LESS;
                                        break;
                        case '>':
                                        next_state = T_GREATER;
                                        break;
                        case '"':
                                        lex_length=0;
                                        next_state = T_LIT;
                                        break;
                        case '.':
                                        set_token( token, E_CONCAT, lex_length, NULL);
                                        return;
                        case '(':
                                        
                                        set_token( token, E_LPARENTHESES, lex_length, NULL);
                                        return;
                        case ')':

                                        set_token( token, E_RPARENTHESES, lex_length, NULL);
                                        return;
                        case '{':
 
                                        set_token( token, E_LABRACK, lex_length, NULL);
                                        return;
                        case '}':

                                        set_token( token, E_RABRACK, lex_length, NULL);
                                        return;
                        case '!':
                                        next_state = T_EXCLAM;
                                        break;
                        case ',':
                                        set_token( token, E_COMA, lex_length, NULL);
                                        return;	                                      
                        case 0:
                                        set_token( token, E_EOF, lex_length, NULL);
                                        return;
                        default:
                                        set_token( token, E_INVLD, lex_length, NULL);
                                        return;
                    } // switch
                    break;
                }
            } // init
            case T_ID:
            {
                lex_length++;
                while( isalnum( znak ) || znak == '_' )
                {
                    znak = getc( current_pos );
                    lex_length++;
                }
                ungetc( current_pos );
                set_token( token, token->ttype, lex_length, current_pos-lex_length);
                is_keyword(current_pos-lex_length, token, lex_length);
                return;
            }
            case T_INT:
            {    
                while( isdigit( znak ) )
                {
                    lex_length++;
                    znak = getc( current_pos );
                }
                if( znak == '.' )
                {
                    next_state = T_FLOAT;
                    lex_length++;
                    break;
                }
                else if( znak == 'e' || znak == 'E' )
                {
                    next_state = T_EXP;
                    lex_length++;
                    break;                    
                }
                else if( is_divider( znak, number_divider ) ) // validny oddelovac cisla
                {
                    ungetc( current_pos );
                    set_token( token, E_INT, lex_length, NULL );
                    if( sscanf( current_pos-lex_length, "%d", &token->data._int ) == 1)
                        return;
                    else
                        set_token( token, E_INVLD, lex_length, NULL );
                    return;
                }
                else
                {
                    ungetc( current_pos );
                    set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
                break;
            }
            case T_ASS: // =
            {
                if ( znak == '=' )               // ==
                {
                    znak = getc( current_pos );
                    //printf( "t_ass %c", znak );
                    if( znak == '=' )            // ===
                    {
                        if( getc( current_pos ) != '=' )
                        {
                            ungetc( current_pos );
                            set_token( token, E_TRIPLEEQ, lex_length, NULL);
                        }
                        else
                            set_token( token, E_INVLD, lex_length, NULL);    // ====
                    }
                    else if( is_divider( znak, operator_divider ) )
                    {
                        set_token( token, E_INVLD, lex_length, NULL);
                        ungetc( current_pos );
                        return;
                    }

                }
                else if( is_divider( znak, operator_divider ) )
                {
                    set_token( token, E_EQ, lex_length, NULL);
                    ungetc( current_pos );
                }
                else
                    set_token( token, E_INVLD, lex_length, NULL);
                
                return;
            }
            case T_BLOCK_C:
            {
                do{
                    if( znak == 0 )
                    {
                        set_token( token, E_EOF, lex_length, NULL);
                        return;
                    }
                    else if( znak == '*' )
                    {
                        znak = getc( current_pos );
                        if( znak == '/' )
                            break;
                        else
                            ungetc( current_pos );
                    }
                    else if( znak == '\n' )
                        scanner_line++;
                    znak = getc( current_pos );
                }while(1);

                next_state = INIT;
                break;
            }
            case T_GREATER:
            {
                if( znak == '=' )
                { // >=?
                    if( is_divider( getc( current_pos ), operator_divider ) )
                        set_token( token, E_GREATEREQ, lex_length, NULL);
                    else
                        set_token( token, E_INVLD, lex_length, NULL);
                    // ungetc( current_pos );
                    return;
                }
                else if( is_divider( znak, operator_divider ) )
                {
                    ungetc( current_pos );
                    set_token( token, E_GREATER, lex_length, NULL);
                }
                else
                    set_token( token, E_INVLD, lex_length, NULL);

                return;
            }
            case T_LESS:
            {
                if( znak == '=' )
                { // <=?
                    if ( is_divider( getc( current_pos ), operator_divider ) )
                    {
                        ungetc( current_pos );
                        set_token( token, E_LESSEQ, lex_length, NULL);
                    }
                    else
                        set_token( token, E_INVLD, lex_length, NULL);
                }
                else if ( is_divider( znak, operator_divider ) || znak == '?' ) // koli <? php
                {
                    ungetc( current_pos );
                    set_token( token, E_LESS, lex_length, NULL);
                }
                else
                    set_token( token, E_INVLD, lex_length, NULL);
                return;
            }
            case T_FRACTION:
            {
                if( znak == '/' )
                {
                    while( (znak = getc( current_pos )) != '\n') // preskoc vsetko az do konca riadku
                    {  
                        if(znak == 0)
                        {
                            set_token(token, E_EOF, lex_length, NULL);
                            return;
                        }
                    }
                    scanner_line++;
                    next_state = INIT;
                    break;
                }
                else if( znak == '*' )
                {
                    next_state = T_BLOCK_C;
                    break;
                }
                else if( is_divider( znak, operator_divider ) )
                {
                    ungetc( current_pos );
                    lex_length--;
                    set_token( token, E_DIV, lex_length, NULL);
                    return;
                }
                else
                {
                    set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
            }
            case T_CONCAT:
            {
                if( is_divider( znak, operator_divider ) )
                    set_token( token, E_CONCAT, lex_length, NULL);
                else
                    set_token( token, E_INVLD, lex_length, NULL);
                return;
            }
            case T_EXCLAM:
            {
                if( znak == '=' && getc( current_pos ) == '=' && getc( current_pos ) != '=' ) // skratove vyhodnocovanie
                    set_token( token, E_NOT_EQ, lex_length, NULL);
                else
                {
                    ungetc( current_pos );
                    set_token( token, E_INVLD, lex_length, NULL);
                }
                return;
            }
            case T_FLOAT:
            {    //123.znak
                if(isdigit(znak)) // ak si cislo tak je to fresh
                {
                    znak = getc( current_pos );
                    lex_length++;
                }
                else // inak je to pas
                {
                    set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
            
            
                if( znak == 'e' || znak == 'E' )
                {
                    lex_length++;
                    next_state = T_EXP;
                    break;
                }
                
                while( isdigit( znak ) )
                {
                    lex_length++;
                    znak = getc( current_pos );
                }

                // 123.45+
                if( is_divider( znak, number_divider ) )
                {
                    ungetc( current_pos );
                    set_token( token, E_DOUBLE, lex_length, NULL ); 
                    sscanf( current_pos - lex_length, "%lf", &token->data._double );
                    return;
                }
                else if( znak == 'e' || znak == 'E' )
                {
                    lex_length++;
                    next_state = T_EXP;
                    break;
                }
                else
                {
                    set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
            }
            case T_EXP:
            {
                // optional +/-
                if( znak == '+' || znak == '-' )
                {
                    lex_length++;
                    znak = getc( current_pos );
                }
                // cisla :
                do
				{
                    if( isdigit( znak ) )
                    {
                        lex_length++;
                        znak = getc( current_pos );
                    }
                    else if( znak == 0 ) // ak by skor ako koniec cisla prisiel EOF
                    {
                        set_token( token, E_INVLD, lex_length, NULL);
                        ungetc( current_pos );
                        return;
                    }
                    else
                        break;
                } while(1);
                
                // koniec exponentu:
                if( is_divider( znak, number_divider ) )
                {
                    ungetc( current_pos );
                    set_token( token, E_DOUBLE, lex_length, NULL ); 
                    sscanf( current_pos - lex_length, "%lf", &token->data._double );
                }
                else // 123.5E-3ň
                    set_token( token, E_INVLD, lex_length, NULL);
                return;
            }
            case T_LIT: 
            {
                int offset = -1;
                lex_length++;
                while( znak != '"' ) // koniec stringu
                {
                    lex_length++;
                    if(  znak < ' ' || znak == '$') // znaky ktore sa v retazci nesmu vyskytovat
                    {
                        set_token( token, E_INVLD, lex_length, NULL);
                        return;
                    }
                    
                    if(znak == '\n') // retazec zapisany na viac riadkov
                        scanner_line++;
                    
                    if( znak == '\\' ) // escape sekvencia 
                    {
                        znak = getc( current_pos );
                        lex_length++;
                        switch( znak )
                        {
                            case 0:     
                                set_token( token, E_EOF, lex_length, NULL);
                                return;
                            case '"' :
                            case '$' :
                            case '\\':
                                offset--;
                                break;
                            case 'n' :
                                offset--;
                                znak = '\n';
                                break;
                            case 't' :
                                offset--;
                                znak = '\t';
                                break;
                            case 'x':
                            {
                                int n_a, n_b;
                                n_a = getc( current_pos );
                                lex_length++;
                                if( ( n_a >= '0' && n_a <= '9'  ) || ( n_a >= 'A' && n_a <= 'F' ) )
                                    ; 
                                else if( n_a == 0 )
                                { 
                                    set_token( token, E_EOF, lex_length, NULL);
                                    return;
                                }
                                else
                                {
                                    ungetc(current_pos);
                                    lex_length--;
                                    break; // !!!
                                }  
                                
                                n_b = getc( current_pos );
                                lex_length++;
                                
                                if(( n_b >= '0' && n_b <= '9'  ) || ( n_b >= 'A' && n_b <= 'F' )) // n_b je platna hexadecimalne cislice 
                                {
                                    znak = hex2int(n_a, n_b);
                                    offset -= 3;
                                }
                                else if( n_b == 0 )
                                { 
                                    set_token( token, E_EOF, lex_length, NULL);
                                    return;
                                }
                                else // nie je to hexa cislo => string bez zmeny
                                {
                                   ungetc(current_pos);
                                   ungetc(current_pos);
                                   lex_length -= 2;
                                   break;
                                }
                                break;
                            }
                            default: // bez zmeny
                                *(current_pos + offset -1) = '\\';
                                break;
                        }
                    } // escape sekvencia
                    *(current_pos + offset) = znak;
                    znak = getc( current_pos );
                    
                } //while

                int count = offset;
                current_pos += offset;
                while(offset != -1)
                {
                    *current_pos++ = ' ';
                    lex_length--;
                    offset++;
                }
                *current_pos = ' ';
                current_pos += (count+1);
                *current_pos++ = '\0';
                
                set_token( token, E_LITER, lex_length-1, current_pos - lex_length);
                return;
            } // T_LIT

            default:
                break;
        }
    }while( 1 );
}
