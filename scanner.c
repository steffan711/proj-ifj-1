/**
 * @file scanner.c
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
#include <string.h> // strcpy


/**
 * ////////////////////////////////////////////////////////////// POPIS FUNKCIE POPROSIM SEM ////////////////////////////////////////////////
 * 
 * @param ukazatel na zasobnik znakov
 * @param znak ktory treba doplnit
 * @return Uspesnost
 */
E_ERROR_TYPE buffer_push( char znak )
{

    if( Buffer->allocated_size > Buffer->size ) // este sa donho zmesti
    {
        Buffer->ptr[Buffer->size++] = znak;
    }
    else // uz sa nezmesti do koze
    {
        Buffer->allocated_size *= allocation_coeficient;

        Buffer->ptr = realloc( Buffer->ptr,( Buffer->allocated_size ) * sizeof( char ) );
        if( Buffer->ptr == NULL )
            return E_INTERPRET_ERROR;

        Buffer->ptr[Buffer->size++] = znak;
    }
    return E_OK;
}

/** 
 * Funkcia zrusi buffer, po zavolani tejto funkcie nevolat scanner_get_token
 */    
void printBuffer() // vypise aktualny obsah zasobnika
{
    if( Buffer->ptr != NULL )
    {
        printf( "Buffer size : %d\n", Buffer->size );
        for( unsigned i = 0; i < Buffer->size; i++ )
            printf( "%c", Buffer->ptr[i] );
            fflush( stdout );
        printf( "\n" );
    }
    else
        printf( "No string data\n" );
    
}



/**
 * ////////////////////////////////////////////////////////////// POPIS FUNKCIE POPROSIM SEM ////////////////////////////////////////////////
 */
void scanner_shutdown()
{
    if(Buffer == NULL)
        return;
    free(Buffer->ptr);
    free(Buffer);
}



/**
 * Funkcia inicializuje strukturu tokenu potrebnymi datami
 *
 * @param ukazatel na token
 * @param token type
 * @param ukazatel (niekde)
 */
static inline void set_token( T_token* token, TOKEN_TYPE type, void* data_ptr )
{
    token->ttype = type;
    token->line = scanner_line;
    token->column = scanner_column;

    if( data_ptr )
    {
        switch( token->ttype )
        {
            case E_DOUBLE:
                {
                    if( sscanf( ( char* ) data_ptr, "%lf", &token->data._double ) == 1 )
                        return;
                    else 
                        token->ttype = E_INVLD;
                }
            case E_INT:
                {
                    if( sscanf( ( char* ) data_ptr, "%d", &token->data._int ) == 1)
                        return;
                    else
                        token->ttype = E_INVLD;
                }
            default:
                {
                    token->data._string = malloc( ( Buffer->size ) * sizeof( char ) + 1 ); //ukoncenie
                    if( token->data._string == NULL )
                    {
                        //perror("Fuck:");
                        exit( E_INTERPRET_ERROR ); // TODO: co s tymto ?
                    }
                    strcpy( token->data._string, ( char* ) data_ptr ); // Ze vraj to tam vlozi aj ukoncovaciu 0
                    break;
                }
        }
    }
    else
        token->data._string = NULL;
}


/**
 * ////////////////////////////////////////////////////////////// POPIS FUNKCIE POPROSIM SEM ////////////////////////////////////////////////
 *
 * @param nacitany oddelovac
 * @param specifikacia mnoziny validnych oddelovacov
 * @return rozhodne ci je token spravne ukonceny
 */
static inline bool is_divider( char znak, int switcher )
{
    switch( switcher )
    {
        case number_divider:    return ( isspace( znak ) || znak == '/' || znak == '+' || znak == '-' || znak == '*' || znak == ';' || znak == ',' || znak == 0) ? true : false;
        case operator_divider:  return ( isspace( znak ) || znak == '$' || isdigit( znak ) || znak == 0 ) ? true : false;
        default: return false;
    }
}
// test: /* /a /7 /$



/** 
 * //////////////////////////////////////////////////////////////POPIS FUNKCIE POPROSIM SEM
 * @param ukazatel na subor
 * @return Uspesnost
 */
E_ERROR_TYPE scanner_init( char *file_start )
{
    Buffer = &stack;
    current_pos = file_start;
    scanner_line = 1;
    scanner_column = 0;
    file_origin = file_start;

    Buffer = malloc( sizeof( tStringBuffer ) );
    if( Buffer == NULL ) // kriste pane ak sa nepodari ani scanner init tak to je v riti...
        return E_INTERPRET_ERROR;

    Buffer->ptr = calloc( pre_allocation * sizeof( char ), sizeof( char ) );
    if( Buffer->ptr == NULL )
        return E_INTERPRET_ERROR;    

    Buffer->allocated_size = pre_allocation;
    Buffer->size = 0;

    return E_OK;
}


/**
 * //////////////////////////////////////////////////////////////POPIS FUNKCIE POPROSIM SEM
 * @param token
 */
void print_token( T_token* token )
{
    printBuffer();
    printf( "\nToken -> column: %u", token->column );
    printf( "\nToken -> line: %u", token->line );

    if     ( token->ttype == E_INT )
        printf( "\nToken -> data: %d", token->data._int );
    else if( token->ttype == E_DOUBLE )
        printf( "\nToken -> data: %e", token->data._double );
    else if( token->data._string )
        printf( "\nToken -> data: %s", token->data._string );
    else
        printf( "\nToken -> data: NO DATA" );

    switch( token->ttype )
    {
        case E_EQ:          printf( "\nToken -> ttype: E_EQ\n" );           break;
        case E_TRIPLEEQ:    printf( "\nToken -> ttype: E_TRIPLEEQ\n" );     break;
        case E_NOT_EQ:      printf( "\nToken -> ttype: E_NOT_EQ\n" );       break;
        case E_LESS:        printf( "\nToken -> ttype: E_LESS\n" );         break;
        case E_GREATER:     printf( "\nToken -> ttype: E_GREATER\n" );      break;
        case E_LESSEQ:      printf( "\nToken -> ttype: E_LESSEQ\n" );       break;
        case E_GREATEREQ:   printf( "\nToken -> ttype: E_GREATEREQ\n" );    break;
        case E_PLUS:        printf( "\nToken -> ttype: E_PLUS\n" );         break;
        case E_MINUS:       printf( "\nToken -> ttype: E_MINUS\n" );        break;
        case E_MULT:        printf( "\nToken -> ttype: E_MULT\n" );         break;
        case E_DIV:         printf( "\nToken -> ttype: E_DIV\n" );          break;
        case E_SEMICL:      printf( "\nToken -> ttype: E_SEMICL\n" );       break;
        case E_COMA:        printf( "\nToken -> ttype: E_COMA\n" );         break;
        case E_CONCAT:      printf( "\nToken -> ttype: E_CONCAT\n" );       break;
        case E_IDENT:       printf( "\nToken -> ttype: E_IDENT\n" );        break;
        case E_VAR:         printf( "\nToken -> ttype: E_VAR\n" );          break;
        case E_INT:         printf( "\nToken -> ttype: E_INT\n" );          break;
        case E_DOUBLE:      printf( "\nToken -> ttype: E_DOUBLE\n" );       break;
        case E_STRING:      printf( "\nToken -> ttype: E_STRING\n" );       break;
        case E_LPARENTHESES: printf( "\nToken -> ttype: E_LPARENTHESES\n" ); break;
        case E_RPARENTHESES: printf( "\nToken -> ttype: E_RPARENTHESES\n" ); break;  
        case E_LABRACK:     printf( "\nToken -> ttype: E_LABRACK\n" );      break;
        case E_RABRACK:    printf( "\nToken -> ttype: E_RABLRACK\n" );      break;
        case E_INVLD:       printf( "\nToken -> ttype: E_INVLD\n" );        break;
        case E_LITER:       printf( "\nToken -> ttype: E_LITER\n" );        break;
        case E_EOF:         printf( "\nToken -> ttype: E_EOF\n" );          break;
        case E_WHILE:       printf( "\nToken -> ttype: E_WHILE\n" );        break;
        case E_FUNCTION:    printf( "\nToken -> ttype: E_FUNCTION\n" );     break;
        case E_FALSE:       printf( "\nToken -> ttype: E_FALSE\n" );        break;
        case E_NULL:        printf( "\nToken -> ttype: E_NULL\n" );         break;
        case E_TRUE:        printf( "\nToken -> ttype: E_TRUE\n" );         break;
        case E_IF:          printf( "\nToken -> ttype: E_IF\n" );           break;
        case E_ELSE:        printf( "\nToken -> ttype: E_ELSE\n" );         break;
        case E_RETURN:      printf( "\nToken -> ttype: E_RETURN\n" );       break;  
        default:    break;
    }
}


/**
 * //////////////////////////////////////////////////////////////POPIS FUNKCIE POPROSIM SEM
 */
void buffer_init()
{
    for( unsigned i = 0; i < Buffer->size; i++ )
        Buffer->ptr[i] = '\0';
    Buffer->size = 0;
}

/**
bool is_keyword();
testuje string na klucove slovo

*/

bool is_keyword(const char* word, T_token* token)
{
	if (strcmp(word,"function") == 0)
	{
		set_token( token, E_FUNCTION, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"else") == 0)
	{
		set_token( token, E_ELSE, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"if") == 0)
	{
		set_token( token, E_IF, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"false") == 0)
	{
		set_token( token, E_FALSE, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"null") == 0)
	{
		set_token( token, E_NULL, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"true") == 0)
	{
		set_token( token, E_TRUE, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"while") == 0)
	{
		set_token( token, E_WHILE, Buffer->ptr );
		return true;
	}
	else if (strcmp(word,"return") == 0)
	{
		set_token( token, E_RETURN, Buffer->ptr );
		return true;
	}
	return false;

}


/**
 * Po zavolani obsahuje parameter token nasledujuci token
 * Ak token obsahuje stringove data (E_VAR,E_IDENT,E_lit), treba ich uvolnit: free( token.data._string )
 *
 * @param ukazatel na token (return)
 */
void scanner_get_token( T_token* token )
{
    Buffer->size = 0;

    FSM_STATE next_state = INIT;
    set_token( token, E_INVLD, NULL ); // inicializacia
    int znak;

    while ( next_state != FINISH )
    {    
        znak = getc( current_pos );
        scanner_column++;
        while( isspace( znak ) )    // white space skip
        {
            if( znak == '\n' )
            {
                scanner_line++;
                scanner_column = 0;
            }
            znak = getc( current_pos );
        }
        switch( next_state )
        {
            // --------------------------------------------------------
            case INIT:    
            {
                if( isalpha( znak ) || znak == '_' ) // A-Za-z_
                {
                    buffer_push( znak );
                    next_state = T_ID;
                    token->ttype = E_IDENT;
                }
                else if( isdigit( znak ) )
                {
                    buffer_push( znak );
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
                        case '$':
                                        next_state = T_ID;
                                        token->ttype = E_VAR;
                                        break;
                        case '=':
                                        next_state = T_ASS;
                                        break;
                        case '+':
                                        set_token( token, E_PLUS, NULL );
                                        return;
                        case '-':
                                        set_token( token, E_MINUS, NULL );
                                        return;
                        case '*':
                                        set_token( token, E_MULT, NULL );
                                        return;
                        case '<':
                                        next_state = T_LESS;
                                        break;
                        case '>':
                                        next_state = T_GREATER;
                                        break;
                        case '"':
                                        next_state = T_LIT;
                                        break;
                        case '.':
                                        set_token( token, E_CONCAT, NULL );
                                        return;
                        case '(':
                                        buffer_push( znak ); // debug
                                        set_token( token, E_LPARENTHESES, NULL );
                                        return;
                        case ')':
                                        buffer_push(znak);
                                        set_token( token, E_RPARENTHESES, NULL );
                                        return;
                        case '{':
                                        buffer_push( znak );
                                        set_token( token, E_LABRACK, NULL );
                                        return;
                        case '}':
                                        buffer_push( znak );
                                        set_token( token, E_RABRACK, NULL );
                                        return;
                        case '!':
                                        next_state = T_EXCLAM;
                                        break;
                        case ',':
                                        set_token( token, E_COMA, NULL );
                                        return;										
                        case 0:
                                        set_token( token, E_EOF, NULL );
                                        return;
                        default:
                                        buffer_push( znak );
                                        set_token( token, E_INVLD, NULL );
                                        return;
                    } // switch
                    buffer_push( znak );
                } // if
                break;
            } // while

            // --------------------------------------------------------
            case T_ID:
            {
                while( isalnum( znak ) || znak == '_' )
                {
                    buffer_push( znak );
                    scanner_column++;
                    znak = getc( current_pos );
                }
                ungetc( current_pos );
                Buffer->ptr[Buffer->size] = '\0';
				unsigned a = strlen(Buffer->ptr);
				if( a > 1 && a < 8 ) // 8 = strlen("function")
				{
					if(is_keyword(Buffer->ptr, token))
						return;
					else
						set_token( token, token->ttype, Buffer->ptr );
				}
                return;
            }

            // --------------------------------------------------------
            case T_INT:
            {    
                while( isdigit( znak ) )
                {
                    buffer_push( znak );
                    znak = getc( current_pos );
                }
                if( znak == '.' )
                {
                    next_state = T_FLOAT;
                    buffer_push( znak );
                    break;
                }
                else if( znak == 'e' || znak == 'E' )
                {
                    next_state = T_EXP;
                    buffer_push( znak );
                    break;                    
                }
                else if( is_divider( znak, number_divider ) ) // vlaidny oddelovac integera
                {
                    ungetc( current_pos );
                    set_token( token, E_INT, Buffer->ptr );
                    return;
                }
                else
                {
                    ungetc( current_pos );
                    set_token( token, E_INVLD, NULL );
                    return;
                }
                break;
            }

            // --------------------------------------------------------
            
            case T_ASS:                          // =
            {
                if ( znak == '=' )               // ==
                {
                    znak = getc( current_pos );
                    printf( "t_ass %c", znak );
                    if( znak == '=' )            // ===
                    {
                        if( getc( current_pos ) != '=' )
                        {
                            ungetc( current_pos );
                            set_token( token, E_TRIPLEEQ, NULL );
                        }
                        else
                            set_token( token, E_INVLD, NULL );    // ====
                    }
                    else if( is_divider( znak, operator_divider ) )
                    {
                        set_token( token, E_INVLD, NULL );
                    }
                    return;
                }
                else if( is_divider( znak, operator_divider ) )
                {
                    set_token( token, E_EQ, NULL );
                }
                return;
            }

            // --------------------------------------------------------
            case T_BLOCK_C:
            {
                while(1)
                {
                    if( znak == 0 )
                    {
                        set_token( token, E_EOF, NULL );
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
                    {
                        scanner_line++;
                        scanner_column = 0;
                    }
                    znak = getc( current_pos );
                }

                next_state = INIT;
                break;
            }

            // --------------------------------------------------------
            case T_GREATER:
            {
                if( znak == '=' )
                { // >=?
                    if( is_divider( getc( current_pos ), operator_divider ) )
                        set_token( token, E_GREATEREQ, NULL );
                    else
                        set_token( token, E_INVLD, NULL );
                }
                else if( is_divider( znak, operator_divider ) )
                {
                    ungetc( current_pos );
                    set_token( token, E_GREATER, NULL );
                }
                else
                    set_token( token, E_INVLD, NULL );

                return;
            }

            // --------------------------------------------------------
            case T_LESS:
            {
                if( znak == '=' )
                { // <=?
                    if ( is_divider( getc( current_pos ), operator_divider ) )
                    {
                        ungetc( current_pos );
                        set_token( token, E_LESSEQ, NULL );
                    }
                    else
                        set_token( token, E_INVLD, NULL );
                }
                else if ( is_divider( znak, operator_divider ) || znak == '?' ) // koli <? php
                {
                    ungetc( current_pos );
                    set_token( token, E_LESS, NULL );
                }
                else
                    set_token( token, E_INVLD, NULL );
                return;
            }

            // --------------------------------------------------------
            case T_FRACTION:
            {
                if( znak == '/' )
                {
                    while( getc( current_pos ) != '\n' ) {}; // preskoc vsetko az do konca riadku
                    scanner_line++;
                    next_state = INIT;
                    break;
                }
                else if( znak == '*' )
                {
                    Buffer->size--;    // pop
                    next_state = T_BLOCK_C;
                    break;
                }
                else if( is_divider( znak, operator_divider ) )
                {
                    ungetc( current_pos );
                    set_token( token, E_DIV, NULL );
                    return;
                }
                else
                {
                    set_token( token, E_INVLD, NULL );
                    return;
                }
            }

            // --------------------------------------------------------
            case T_CONCAT:
            {
                if( is_divider( znak, operator_divider ) )
                    set_token( token, E_CONCAT, NULL );
                else
                    set_token( token, E_INVLD, NULL );
                return;
            }

            // --------------------------------------------------------
            case T_EXCLAM:
            {
                if( znak == '=' && getc( current_pos ) == '=' )
                    set_token( token, E_NOT_EQ, NULL );
                else
                {
                    ungetc( current_pos );
                    set_token( token, E_INVLD, NULL );
                }
                return;
            }

            // --------------------------------------------------------
            case T_FLOAT:
            {    //123.znak
                if( znak == 'e' || znak == 'E' )
                {
                    buffer_push( znak );
                    next_state = T_EXP;
                    break;
                }
                
                while( isdigit( znak ) )
                {
                    buffer_push( znak );
                    znak = getc( current_pos );
                }

                // 123.45+
                if( is_divider( znak, number_divider ) )
                {
                    set_token( token, E_DOUBLE, Buffer->ptr );
                    ungetc( current_pos );
                    return;
                }
                else if( znak == 'e' || znak == 'E' )
                {
                    buffer_push( znak );
                    next_state = T_EXP;
                    break;
                }
                else
                {
                    set_token( token, E_INVLD, NULL );
                    return;
                }
            }

            // --------------------------------------------------------
            case T_EXP:
            {
                // optional +/-
                if( znak == '+' || znak == '-' )
                {
                    buffer_push( znak );
                    znak = getc( current_pos );
                }
                // cisla :
                do
				{
                    if( isdigit( znak ) )
                    {
                        buffer_push( znak );
                        znak = getc( current_pos );
                    }
                    else if( znak == 0 ) // ak by skor ako koniec cisla prisiel EOF
                    {
                        set_token( token, E_INVLD, NULL );
                        ungetc( current_pos );
                        return;
                    }
                    else
                        break;
                } while(1);
                
                // koniec exponentu:
                if( is_divider( znak, number_divider ) )
                {
                    // 123.45E-23 123.45E23
                    ungetc( current_pos );
                    set_token( token, E_DOUBLE, Buffer->ptr );
                }
                else // 123.5E-3ň
                    set_token( token, E_INVLD, NULL );
                return;
            }

            // --------------------------------------------------------
            case T_LIT:
            {
                while( znak != '"' )
                {
                    if( znak == '\\' )
                    {
                        next_state = T_ESCAPE;
                        break;
                    }
                    
                    if( znak == 0 )
                    {
                        set_token( token, E_INVLD, NULL );
                        ungetc( current_pos );
                        return;
                    }

                    if( znak == '$' ) // dolar musi byt cez escape
                    {
                        set_token( token, E_INVLD, NULL );
                        return;
                    }
                    buffer_push( znak );
                    znak = getc( current_pos );
                }

                if( next_state == T_ESCAPE )
                    break;
                else
                {
                    buffer_push( znak ); // ulozime "
                    set_token( token, E_LITER, Buffer->ptr );
                    return;
                }
            }

            // --------------------------------------------------------
            case T_ESCAPE:
            {
                if( znak < ' ' ) // < 31
                {
                    set_token( token, E_INVLD, NULL );
                    return;
                }

                char hexa_cislo[1];
                unsigned cislo;

                switch( znak )
                {
                    case '"' :
                    case '$' :
                    case '\\':
                                buffer_push( znak );
                                break;

                    case 'x':
                                hexa_cislo[0] = getc( current_pos );
                                if( hexa_cislo[0] != 0 )
                                    hexa_cislo[1] = getc( current_pos );
                                else
                                {   // ak by na konci suboru bolo "\x tak by nas mohol navstivit ujo segfault
                                    set_token( token, E_EOF, NULL );
                                    return;
                                }

                                if( sscanf( hexa_cislo, "%x", &cislo ) == 1 )
                                {
                                    buffer_push( cislo );
                                }
                                else
                                {
                                    set_token( token, E_INVLD, NULL );
                                    return;
                                }
                                break;

                    case 'n':
                                buffer_push(10); // \n
                                break;

                    case 't':
                                buffer_push(11); // \t
                                break;

                    default:
                                buffer_push('\\'); // escape sekvencia nemoze sposobit chybu
                                buffer_push( znak ); // cili to tam proste pushnem tak jak je a jeee
                                break;
                }
                next_state = T_LIT;
                break;
            }
            default:
            {
                next_state = FINISH;
                return;
            }
        }
    } // while
} // function
