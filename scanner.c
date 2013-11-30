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
#define TRUE 1

#define ishexa( x ) ( ( x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F' ) )

 /** Globalne premenne **/
 char*          current_pos;    // aktualna pozicia scannera v subore
 unsigned       scanner_line;   // aktualne spracovavany riadok
 const char*    end_ptr;        // zaciatok suboru v pamati

/**
 * Funkcia realizuje prevod hexadecimalneho cisla na integer
 */
static inline int hex2int( char a, char b )
{
    int c;
   
    if ( a <= '9' )             // cislo
        c = ( a-'0')*16;
    else if ( a >= 'a' )        // male pismeno
        c = ( a-'a'+10 )*16;
    else                        // inak bolo velke pismeno
        c = ( a-'A'+10 )*16;
    /* druhy znak */
    if ( b <= '9' )
        c += ( b-'0');
    else if ( b >= 'a' )
        c += ( b-'a'+10 );
    else
        c += ( b-'A'+10 );
    return c;
}




/**
 * @brief inicializuje scanner pred jeho prvym pouzitim
 * @param [in] ukazatel na subor
 * @param [in] velkost suboru
*/
extern inline void scanner_init( char *file_start, unsigned file_size )
{
    current_pos = file_start;
    scanner_line = 1;
    end_ptr = file_start + file_size + 1; 
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
 * Funkcia naplni strukturu tokenu potrebnymi datami
 *
 * @param [out] ukazatel na token
 * @param [in] token type
 * @param [in] dlzka lexemy
 * @param [in] atribut
 */
static inline void set_token( T_token* token, TOKEN_TYPE type, unsigned dlzka, void* data )
{
    token->line = scanner_line;
    token->length = dlzka;
    token->ttype = type;
    token->data._string = data;
}


/**
 * Po zavolani obsahuje parameter token nasledujuci token
 *
 * @param [out] ukazatel na token
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
            
            if( znak == '$' )
                next_state = T_VAR;
            else if( isalpha( znak ) || znak == '_' ) // A-Za-z_
                next_state = T_ID;
            else if( isdigit( znak ) ) // 0-9
                next_state = T_INT;
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
                                    lex_length = 0;
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
                    case EOF:       if ( current_pos == end_ptr ) // end of file check
                                       set_token( token, E_EOF, lex_length, NULL); 
                                    else
                                        set_token( token, E_INVLD, lex_length, NULL);
                                    return;
                    default:        set_token( token, E_INVLD, lex_length, NULL);
                                    return;
                } // switch znak
            }
        } // init
        else
            switch( next_state )
            {
                case T_ID: // identifikator
                {
                    lex_length++;
                    while( isalnum( znak ) || znak == '_' )
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    }
                    ungetc( current_pos );  
                    set_token( token, E_IDENT, lex_length, current_pos-lex_length);
                    
                    /** Porovnanie s klucovymi slovami **/
                    switch(*(current_pos - lex_length)) // prvy znak identifikatoru
                    {
                        case 'e':   if ( sstrcmp( current_pos - lex_length, "else", lex_length, 4 ) == 0 ) 
                                        set_token( token, E_ELSE, 0, NULL);     
                                    else if ( sstrcmp( current_pos - lex_length, "elseif", lex_length, 6 ) == 0 )
                                        set_token( token, E_ELSEIF, 0, NULL);                                                     
                                    return;
                        case 'n':   if ( sstrcmp( current_pos - lex_length, "null", lex_length, 4 ) == 0 )
                                        set_token( token, E_NULL, 0, NULL);
                                    return;
                        case 't':   if ( sstrcmp( current_pos - lex_length, "true", lex_length, 4 ) == 0 )
                                        set_token( token, E_TRUE, 0, NULL);
                                    return;
                        case 'i':   if ( sstrcmp( current_pos - lex_length, "if", lex_length, 2 ) == 0 )
                                        set_token( token, E_IF, 0, NULL);
                                    return;  
                        case 'r':   if ( sstrcmp( current_pos - lex_length, "return", lex_length, 6 ) == 0 )
                                        set_token( token, E_RETURN, 0, NULL);
                                    return;
                        case 'w':   if ( sstrcmp( current_pos - lex_length, "while", lex_length, 5 ) == 0 )
                                        set_token( token, E_WHILE, 0, NULL);
                                    return;
                        case 'f':   if ( sstrcmp( current_pos - lex_length, "false", lex_length, 5 ) == 0 )
                                        set_token( token, E_FALSE, 0, NULL);
                                    else if ( sstrcmp( current_pos - lex_length, "function", lex_length, 8 ) == 0 )
                                        set_token( token, E_FUNCTION, 0, NULL);
                        default:    return;
                    }
                }
                case T_VAR: // premenna
                {
                    if( !( isalpha( znak ) || znak == '_' ) ) // premenna bez mena / s neplatnym menom
                    {
                        set_token( token, E_INVLD, 0, NULL); 
                        return;
                    }            
                        
                    do
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    } while( isalnum( znak ) || znak == '_' );
                    
                    ungetc( current_pos );
                    set_token( token, E_VAR, lex_length, current_pos-lex_length);
                    return;
                }
                case T_INT: // cislo
                {    
                    lex_length++;
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
                case T_BLOCK_C: // komentar
                {
                    do{
                        if( znak == '*' )
                        {
                            znak = getc( current_pos );
                            if( znak == '/' )
                                break;
                            else
                                ungetc( current_pos );
                        }
                        else if( znak == '\n' )
                            scanner_line++;
                        else if( znak == EOF )
                        {
                            ungetc(current_pos);
                            next_state = INIT;
                            break;
                        } 
                        znak = getc( current_pos );
                    }while(TRUE);
    
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
                    if( znak == '/' ) // riadkovy komentar
                    {
                        while( (znak = getc( current_pos )) != '\n') // preskoc vsetko az do konca riadku
                        {  
                            if(znak == EOF)
                            {
                                ungetc(current_pos);
                                scanner_line--;
                                break;
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
                        set_token( token, E_NOT_EQ, lex_length, NULL); // !==
                    else
                        set_token( token, E_INVLD, lex_length, NULL);
                    return;
                }
                case T_FLOAT:
                {    //123.znak
                    if( isdigit(znak) ) // ak si cislo tak je to fresh
                    {
                        znak = getc( current_pos );
                        lex_length++;
                    }
                    else // inak je to lexikalna chyba
                    {
                        set_token( token, E_INVLD, lex_length, NULL);
                        return;
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
                    
                    if( isdigit(znak) )
                        lex_length++;
                        
                    else // exponent bez akehokolvek cisla
                    {        
                        set_token( token, E_INVLD, lex_length, NULL ); 
                        return;
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
                    } while(TRUE);
                    
                    // koniec exponentu:
                    ungetc( current_pos );
                    set_token( token, E_DOUBLE, lex_length, NULL ); 
                    sscanf( current_pos - lex_length, "%lf", &token->data._double );
                    return;
                } // T_EXP
                // -----------------------------------------------------------
                case T_LIT: // nacitavanie retazca
                {
                    char * zapisovacia_hlava = current_pos - 1;
                    char * pociatocna_pozicia = zapisovacia_hlava;
                    char a, b;
                    
                    while( znak != '"' ) // koniec retazca
                    {
                        if( znak == '\\' ) // escape sekvencia
                        {
                            *zapisovacia_hlava++ = znak; 
                            znak = getc( current_pos );
                            switch( znak )
                            {
                                case 'n':
                                    *(zapisovacia_hlava - 1) = '\n';
                                    znak = getc( current_pos );
                                    break;
                                case 't':
                                    *(zapisovacia_hlava - 1) = '\t';
                                    znak = getc( current_pos );
                                    break;
                                case '\\':
                                case '"':
                                case '$':
                                    *(zapisovacia_hlava - 1) = znak;
                                    znak = getc( current_pos );
                                    break;
                                case 'x':
                                    *zapisovacia_hlava++ = znak; 
                                    a = znak = getc( current_pos );
                                    if ( ishexa(a) )
                                    {
                                        *zapisovacia_hlava++ = znak; 
                                        b = znak = getc( current_pos );
                                        if ( ishexa(b) )
                                        {
                                            zapisovacia_hlava -= 3;
                                            *zapisovacia_hlava++ = hex2int( a, b );
                                            znak = getc( current_pos );
                                        }
                                    }
                                    break;
                                
                                default:
                                    if ( znak < ' ' || znak == '$' )
                                    {
                                        set_token( token, E_INVLD, lex_length, NULL ); 
                                        return;
                                    }
                                    *zapisovacia_hlava++ = znak; 
                                    znak = getc( current_pos );
                                    break;
                            }
                        }
                        else if ( znak < ' ' || znak == '$' ) // znaky, ktore nemozu byt sucastou retazca
                        {
                            set_token( token, E_INVLD, lex_length, NULL ); 
                            return;
                        }
                        else
                        {
                            *zapisovacia_hlava++ = znak; 
                            znak = getc( current_pos );
                        }
                    }
                    set_token( token, E_LITER, zapisovacia_hlava - pociatocna_pozicia , pociatocna_pozicia);
                    return;
                } // T_LIT
                // -----------------------------------------------------------
                default:
                    break;
            } // switch next state
    }while( TRUE );
}
