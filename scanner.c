/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xnemcek03, xilavsk01, xmarti61
 * Subor scanner.c Modul lexikalneho analyzatoru
 *
 * @brief Modul lexikalneho analyzatoru
 * @author Vladimír Čillo, xcillo00
 */


#include <ctype.h> // isdigit, isspace, isalnum
#include "file_io.h"
#include "scanner.h"
#include "debug.h"


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
        if( next_state == INIT )
        {
            while( isspace( znak ) )    // white space skip
            {
                if( znak == '\n' )
                    scanner_line++;
                znak = getc( current_pos );
            }
            
            if( znak == '$')
            {
                next_state = T_VAR;
                lex_length--; // $ nie je sucastou mena premennej
            }
            else if( isalpha( znak ) || znak == '_' ) // A-Za-z_
            {
                next_state = T_ID;
                token->ttype = E_IDENT;
            }
            else if( isdigit( znak ) ) // 0-9
            {
                lex_length++;
                token->ttype = E_INT;
                next_state = T_INT;
            }
            else
            {
                switch( znak )
                {        
                    case '/':
                                    next_state = T_FRACTION;
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
                    case EOF:
                                    set_token( token, E_EOF, lex_length, NULL);
                                    return;
                    default:
                                    set_token( token, E_INVLD, lex_length, NULL);
                                    return;
                } // switch znak
            }
        } // init
        else
            switch( next_state )
            {
                case T_ID:
                {
                    lex_length++;
                    while( isalnum( znak ) || znak == '_' )
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    }
                    ungetc( current_pos );  
                    /** Porovnanie s klucovymi slovami**/
                    switch(*(current_pos - lex_length)) // prvy znak identifikatoru
                    {
                        case 'e':   if ( sstrcmp( current_pos - lex_length, "else", lex_length, 4 ) == 0 ) 
                                        set_token( token, E_ELSE, lex_length, NULL);
                                    return;
                        case 'n':   if ( sstrcmp( current_pos - lex_length, "null", lex_length, 4 ) == 0 )
                                        set_token( token, E_NULL, lex_length, NULL);
                                    return;
                        case 't':   if ( sstrcmp( current_pos - lex_length, "true", lex_length, 4 ) == 0 ) 
                                        set_token( token, E_TRUE, lex_length, NULL);
                                    return;
                        case 'i':   if ( sstrcmp( current_pos - lex_length, "if", lex_length, 2 ) == 0 )
                                        set_token( token, E_IF, lex_length, NULL);
                                    return;            
                        case 'r':   if ( sstrcmp( current_pos - lex_length, "return", lex_length, 6 ) == 0 )
                                        set_token( token, E_RETURN, lex_length, NULL);
                                    return;
                        case 'w':   if ( sstrcmp( current_pos - lex_length, "while", lex_length, 5 ) == 0 )
                                        set_token( token, E_WHILE, lex_length, NULL);
                                    return;
                        case 'f':   if ( sstrcmp( current_pos - lex_length, "false", lex_length, 5 ) == 0 )
                                        set_token( token, E_FALSE, lex_length, NULL);
                                    else if ( sstrcmp( current_pos - lex_length, "function", lex_length, 8 ) == 0 )
                                        set_token( token, E_FUNCTION, lex_length, NULL);
                                    return;                    
                        default:    set_token( token, E_IDENT, lex_length, current_pos-lex_length);
                                    return;
                    }     
                }
                case T_VAR:
                {
                    lex_length++;
                    if( isalpha( znak = getc( current_pos ) ) || znak == '_' )
                        lex_length++;
                    do
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    }while( isalnum( znak ) || znak == '_' );
                    ungetc( current_pos );
                    set_token( token, E_VAR, lex_length, current_pos-lex_length);
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
                    else
                    {
                        ungetc( current_pos );
                        set_token( token, E_INT, lex_length, NULL );
                        sscanf( current_pos-lex_length, "%d", &token->data._int );
                        return;
                    }
                }
                case T_ASS: // =
                {
                    if ( znak == '=' )  // ==
                    {
                        if( ( znak = getc( current_pos ) ) == '=' ) // === ?
                             set_token( token, E_TRIPLEEQ, lex_length, NULL);
                        else // nie je to token ===
                            set_token( token, E_INVLD, lex_length, NULL);
                    }
                    else 
                    {   
                        ungetc( current_pos );
                        set_token( token, E_EQ, lex_length, NULL);
                    }
                    return;
                }
                case T_BLOCK_C:
                {
                    do{
                        if( znak == EOF )
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
                        set_token( token, E_GREATEREQ, lex_length, NULL);
                    else 
                    {
                        ungetc( current_pos );
                        set_token( token, E_GREATER, lex_length, NULL);
                    }
                    return;
                }
                case T_LESS:
                {
                    if( znak == '=' ) // <=
                            set_token( token, E_LESSEQ, lex_length, NULL);
                    else
                    {
                        ungetc( current_pos );
                        set_token( token, E_LESS, lex_length, NULL);
                    }
                    return;
                }
                case T_FRACTION:
                {
                    if( znak == '/' ) // token //
                    {
                        while( (znak = getc( current_pos )) != '\n') // preskoc vsetko az do konca riadku
                        {  
                            if(znak == EOF)
                            {
                                set_token(token, E_EOF, lex_length, NULL);
                                return;
                            }
                        }
                        scanner_line++;
                        next_state = INIT;
                        break;
                    }
                    else if( znak == '*' ) // zaciatok blokoveho komentara
                    {
                        next_state = T_BLOCK_C;
                        break;
                    }
                    else // operator delenia
                    {
                        ungetc( current_pos );
                        set_token( token, E_DIV, 0, NULL);
                        return;
                    }
                }
                case T_EXCLAM:
                {
                    if( znak == '=' && getc( current_pos ) == '=')
                        set_token( token, E_NOT_EQ, lex_length, NULL);
                    else
                        set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
                case T_FLOAT:
                {    //123.znak
                    if(isdigit(znak)) // ak si cislo tak je to fresh
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    }
                    else // inak je to lexikalna chyba
                    {
                        set_token( token, E_INVLD, lex_length, NULL);
                        return;
                    }
                    //123.4znak
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
                    // 123.45678znak
                    if( znak == 'e' || znak == 'E' )
                    {
                        lex_length++;
                        next_state = T_EXP;
                        break;
                    }
                    else
                    {
                        ungetc( current_pos );
                        set_token( token, E_DOUBLE, lex_length, NULL ); 
                        sscanf( current_pos - lex_length, "%lf", &token->data._double );
                        return;
                    }
                }
                case T_EXP: // nacitavanie exponentu
                {
                    if( znak == '+' || znak == '-' ) // optional +/-
                    {
                        lex_length++;
                        znak = getc( current_pos );
                    }
                    
                    do // cisla exponentu :
                    {
                        if( isdigit( znak ) )
                        {
                            lex_length++;
                            znak = getc( current_pos );
                        }
                        else
                            break;
                    } while(1);
                    
                    // koniec exponentu:
                    ungetc( current_pos );
                    set_token( token, E_DOUBLE, lex_length, NULL ); 
                    sscanf( current_pos - lex_length, "%lf", &token->data._double );
                    return;
                } // T_EXP
                case T_LIT: // nacitavanie retazca
                {
                    int offset = -1;
                    lex_length++;
                    while( znak != '"' ) // koniec retazca
                    {
                        lex_length++;
                        if(  znak < ' ' || znak == '$') // znaky ktore sa v retazci nesmu vyskytovat
                        {
                            set_token( token, E_INVLD, lex_length, NULL); // sposobia lexiklanu chybu
                            return;
                        }
                        
                        if( znak == '\\' ) // escape sekvencia 
                        {
                            znak = getc( current_pos );
                            lex_length++;
                            switch( znak )
                            {
                                case EOF:     
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
                                    else if( n_a == EOF )
                                    { 
                                        set_token( token, E_EOF, lex_length, NULL);
                                        return;
                                    }
                                    else
                                    {
                                        ungetc(current_pos);
                                        lex_length--;
                                        break; 
                                    }  
                                    
                                    n_b = getc( current_pos );
                                    lex_length++;
                                    
                                    if(( n_b >= '0' && n_b <= '9'  ) || ( n_b >= 'A' && n_b <= 'F' )) // TODO: rozsirit aj na a-f
                                    {
                                        znak = hex2int(n_a, n_b);
                                        offset -= 3;
                                    }
                                    else if( n_b == EOF )
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
            } // switch next state
    }while( 1 );
}
