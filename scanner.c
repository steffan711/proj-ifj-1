/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xmarti62, xnemce03, xilavs01
 *
 * Subor: scanner.c - modul lexikalneho analyzatoru
 * Autor: Vladimír Čillo, xcillo00
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"
#include <string.h> // strcpy

		
		


/** 1 **
 * @function buffer_push
 * 
 * @param1 ukazatel na zasobnik znakov
 * @param2 znak ktory treba doplnit
 * @return E_OK / E_OTHER
 */	
int buffer_push(char znak) 
{

	if (Buffer->allocated_size > Buffer->size ) // este sa donho zmesti
	{
		Buffer->ptr[Buffer->size++] = znak;
	}
	else // uz sa nezmesti do koze
	{
		Buffer->allocated_size *= allocation_coeficient;
		Buffer->ptr = realloc(Buffer->ptr,(Buffer->allocated_size)*sizeof(char));
		if(Buffer->ptr == NULL)
			return E_INTERPRET_ERROR;
		Buffer->ptr[Buffer->size++] = znak;
	}
	return E_OK;
}

/** 2 **/
void printBuffer() // vypise aktualny obsah zasobnika
{
	if (Buffer->ptr != NULL)
	{
		printf("Buffer size : %d\n", Buffer->size );
		for(unsigned i = 0; i<Buffer->size; i++)
			printf("%c",Buffer->ptr[i]);
			fflush(stdout);
		printf("\n");
	}
	else
		printf("No string data\n");
	
}



/** 3 **
 * @function scanner_shutdown()
 * @
 * @ zrusi buffer, po zavolani tejto funkcie nevolat scanner_get_token
 */	
void scanner_shutdown()
{
	if(Buffer == NULL)
		return;
	free(Buffer->ptr);
	free(Buffer);
}



/** 4 ** 
 * @function set_token
 * Inicializuje strukturu tokenu potrebnymi datami
 * @param1 ukazatel na token
 * @param2 token type
 * @param3 ukazatel (niekde)
 * @return void
 */
static inline void set_token(T_token* ptr, TOKEN_TYPE type, void* data_ptr)
{
	ptr->ttype = type;
	ptr->line = scanner_line;
	ptr->column = scanner_column;

	if (data_ptr)
	{
		switch(ptr->ttype)
		{
			case E_DOUBLE:
				{
					if( sscanf((char*)data_ptr,"%lf",&ptr->data.d) == 1)
						return;
					else 
						ptr->ttype = E_invld;
						
				}
			case E_INT :
				{
					if( sscanf((char*)data_ptr,"%d",&ptr->data.i) == 1)
						return;
					else
						ptr->ttype = E_invld;
				}
			default:
				{
					ptr->data.s = malloc((Buffer->size)*sizeof(char)+1); //ukoncenie
					if (ptr->data.s == NULL)
					{
						//perror("Fuck:");
						exit(E_INTERPRET_ERROR); // TODO: co s tymto ?
					}
					strcpy(ptr->data.s, (char*)data_ptr); // Ze vraj to tam vlozi aj ukoncovaciu 0
					break;
				}
		}
	}
	else
		ptr->data.s = NULL;
		
}
 
 
 
 /** 5 **
 * @function is_divider
 * @ param1 nacitany oddelovac
 * @ param2 specifikacia mnoziny validnych oddelovacov
 * @ rozhodne ci je token spravne ukonceny
 */	
 static inline bool is_divider(char znak, int switcher) 
 {
	switch (switcher)
	{
		case number_divider: 	return ( isspace(znak) || znak == '/' || znak == '+' || znak == '-' || znak == '*' || znak == ';' || znak == ',' || znak == 0) ? true : false;
		case operator_divider:	return ( isspace(znak) || znak == '$' || isdigit(znak) || znak == 0 ) ? true : false;
		default: return false;
	}
	return false;
 }
 // test: /* /a /7 /$
 
 
 

 /** 6 ** @function scanner_init
 * @
 * @param ukazatel na subor
 * @return -
 */	
int scanner_init(char *file_start)
 {
	Buffer = &stack;
	current_pos = file_start;
	scanner_line = 1;
	scanner_column = 0;
	file_origin = file_start;
	Buffer = malloc(sizeof(tStringBuffer));
	if (Buffer == NULL)
	{ // kriste pane ak sa nepodari ani scanner init tak to je v riti...
		return  E_INTERPRET_ERROR;	
	}
	Buffer->ptr = calloc(pre_allocation * sizeof(char),sizeof(char)); 
	if (Buffer->ptr == NULL)
	{ // shit
		return  E_INTERPRET_ERROR;	
	}
	
	Buffer->allocated_size = pre_allocation;
	Buffer->size = 0;

	return E_OK;
 }
 
/** 7 **/
 void print_token(T_token* token)
 {
	printBuffer();
	printf("\nToken -> column: %u",token->column);
	printf("\nToken -> line: %u",token->line);
	if(token-> ttype == E_INT )
		printf("\nToken -> data: %d",token->data.i);
	else if(token-> ttype == E_DOUBLE )
		printf("\nToken -> data: %e",token->data.d);
	else  if(token->data.s)
		printf("\nToken -> data: %s",token->data.s);
	else
		printf("\nToken -> data: NO DATA");
	switch(token->ttype)
	{
		case 1:		printf("\nToken -> ttype: E_EQ\n"); 		break;
		case 2:		printf("\nToken -> ttype: E_COMP\n"); 		break;
		case 3:		printf("\nToken -> ttype: E_tripleeq\n");	break;
		case 4:		printf("\nToken -> ttype: E_not_eq\n"); 	break;
		case 5:		printf("\nToken -> ttype: E_LESS\n"); 		break;
		case 6:		printf("\nToken -> ttype: E_GREATER\n"); 	break;
		case 7:		printf("\nToken -> ttype: E_LESSEQ\n"); 	break;
		case 8:		printf("\nToken -> ttype: E_GREATEREQ\n");  break;
		case 9:		printf("\nToken -> ttype: E_PLUS\n"); 		break;
		case 10:	printf("\nToken -> ttype: E_MINUS\n"); 		break;
		case 11:	printf("\nToken -> ttype: E_MULT\n");		break;
		case 12:	printf("\nToken -> ttype: E_DIV\n");		break;
		case 13:	printf("\nToken -> ttype: E_SEMICL\n");		break;
		case 14:	printf("\nToken -> ttype: E_COMA\n");		break;
		case 15:	printf("\nToken -> ttype: E_CONCAT\n");		break;
		case 16:	printf("\nToken -> ttype: E_IDENT\n");		break;
		case 17:	printf("\nToken -> ttype: E_VAR\n");		break;
		case 18:	printf("\nToken -> ttype: E_INT\n");		break;
		case 19:	printf("\nToken -> ttype: E_DOUBLE\n");		break;
		case 20:	printf("\nToken -> ttype: E_STRING\n");		break;
		case 21:	printf("\nToken -> ttype: E_Lparentheses\n");break;
		case 22:	printf("\nToken -> ttype: E_Rparentheses\n");break;
		case 23:	printf("\nToken -> ttype: E_lBrack\n");		break;
		case 24:	printf("\nToken -> ttype: E_rBrack\n");		break;
		case 25:	printf("\nToken -> ttype: E_laBrack\n");	break;
		case 26:	printf("\nToken -> ttype: E_raBrack\n");	break;
		case 27:	printf("\nToken -> ttype: E_invld\n");		break;
		case 28:	printf("\nToken -> ttype: E_liter\n");		break;
		case 29:	printf("\nToken -> ttype: E_EOF\n");		break;


		default: break;
	}
	
 }
 
 /** 8 **/
 void buffer_init()
 {
	for(unsigned i = 0; i < Buffer->size; i++)
		Buffer->ptr[i] = '\0';
	Buffer->size = 0;
 }
 
  /** 9 ** 
 * @function scanner_get_token
 * @param (OUT)	ukazatel na token
 * @return Po zavolani obsahuje parameter token nasledujuci token
 * @pamat Ak token obsahuje stringove data (E_VAR,E_IDENT,E_lit), treba ich uvolnit
 * Nejak takto: free(token.data.s)
 */	

void scanner_get_token(T_token* token)
{
	Buffer->size = 0;
	
	FSM_STATE next_state = INIT;
	set_token(token,E_invld,NULL); // inicializacia
	int znak; 
	
	
	while ( next_state != FINISH )
	{	
		znak=getc(current_pos);
		scanner_column++;
		while(isspace(znak))	// white space skip
		{
			if(znak == '\n')
			{
				scanner_line++;
				scanner_column = 0;
			}
			znak=getc(current_pos);			
		}
		switch(next_state)
		{	
			// --------------------------------------------------------	
			case INIT:	
			{
				if(isalpha(znak) || znak == '_') // A-Za-z_
				{
					buffer_push(znak);
					next_state = t_id;
					token->ttype = E_IDENT;
				}
				else if(isdigit(znak))
				{
					buffer_push(znak);
					token->ttype = E_INT;
					next_state = t_int;
				}
				else
				{	
					switch(znak)
					{		
						case '/':	
										next_state = t_fraction;
										break;
						case '$':		
										next_state = t_id;
										token->ttype= E_VAR;
										break;
	
						case '=':	
										next_state = t_ass;
										break;

						case '+':	
										set_token(token,E_PLUS,NULL);
										return;
									
						case '-':	
										set_token(token,E_MINUS,NULL);
										return;
									
						case '*':	
										set_token(token,E_MINUS,NULL);
										return;
									
						case '<':	
										next_state = t_less;
										break;
									
						case '>':	
										next_state = t_greater;
										break;
									
						case '"':	
										next_state = t_lit;
										break;
									
						case '.':	
										set_token(token,E_CONCAT,NULL);
										return;
									
						case '(':	
										buffer_push(znak); // debug
										set_token(token,E_Lparentheses,NULL);
										return;
									
						case ')':	
										buffer_push(znak);
										set_token(token,E_Rparentheses,NULL);
										return;
									
						case '[':	
										buffer_push(znak);
										set_token(token,E_lBrack,NULL);
										return;
																		
						case ']':	
										buffer_push(znak);
										set_token(token,E_rBrack,NULL);
										return;
									
						case '{':	
										buffer_push(znak);
										set_token(token,E_laBrack,NULL);
										return;
									
						case '}':	
										buffer_push(znak);
										set_token(token,E_raBrack,NULL);
										return;
																		
						case '!':	
										next_state = t_exclam;
										break;
									
						case 0:		
										set_token(token,E_EOF,NULL);
										return;
																				
						default: 	
										buffer_push(znak);
										set_token(token,E_invld,NULL);
										return;
									
					}	// switch
					buffer_push(znak);
				}	// if
				break;
			} // while 
			
			// --------------------------------------------------------						
			case t_id:		
			{
				while(isalnum(znak) || znak == '_')
				{	
					buffer_push(znak);
					scanner_column++;
					znak = getc(current_pos);								
				}
				ungetc(current_pos);
				Buffer->ptr[Buffer->size] = '\0';
				set_token(token,token->ttype,Buffer->ptr);
				return;
			}
			
			// --------------------------------------------------------	
			case t_int:		
			{	
				while(isdigit(znak))
				{
					buffer_push(znak);
					znak = getc(current_pos);
				}
				if(znak == '.')
				{
					next_state = t_float;
					buffer_push(znak);
					break;
				}
				else if (znak == 'e' || znak == 'E')
				{
					next_state = t_exp;
					buffer_push(znak);
					break;					
				}
				else if (is_divider(znak,number_divider) ) // vlaidny oddelovac integera
				{
					ungetc(current_pos);
					set_token(token,E_INT,Buffer->ptr);
					return;
				}
				else
				{
					ungetc(current_pos);
					set_token(token,E_invld,NULL);
					return;
				}
				break;
			}
			
			// --------------------------------------------------------	
			
			case t_ass:	// =
			{	
				if (znak == '=') // ==
				{	
					znak = getc(current_pos);
					printf("t_ass %c",znak);
					if(znak == '=') // ===
					{
						if(getc(current_pos) != '=')
						{
							ungetc(current_pos);
							set_token(token,E_tripleeq,NULL);
						}
						else
							set_token(token,E_invld,NULL); // ====
					}
					else if (is_divider(znak,operator_divider))
					{
						set_token(token,E_COMP,NULL);
					}
					return;
				}
				else if(is_divider(znak,operator_divider))
				{
					set_token(token,E_EQ,NULL);
				}
				return;

			}
			
			// --------------------------------------------------------
			
			case t_block_c:	
			{	
				while(1) 
				{
					if( znak == 0 )
					{
						set_token(token,E_EOF,NULL);
						return;
					}
					else if (znak == '*')
					{
						znak = getc(current_pos);
						if( znak == '/')
							break;
						else
							ungetc(current_pos);
					}
					else if(znak == '\n')
					{
						scanner_line++;
						scanner_column = 0;
					}
					znak = getc(current_pos);
				}

				next_state = INIT;
				break;
			}	
													
			// --------------------------------------------------------				
							
			case t_greater:	
			{ 
				if(znak == '=')
				{ // >=?
					if ( is_divider(getc(current_pos),operator_divider ) )
						set_token(token,E_GREATEREQ,NULL);
					else
						set_token(token,E_invld,NULL);
				}
				else if ( is_divider(znak,operator_divider) )
				{

					ungetc(current_pos);
					set_token(token,E_GREATER,NULL);
				}
				else
					set_token(token,E_invld,NULL);
					
				return;
			}	
			// --------------------------------------------------------
							
			case t_less:	
			{
				if(znak == '=')
				{ // <=?
					if ( is_divider(getc(current_pos),operator_divider ) )
					{
						ungetc(current_pos);
						set_token(token,E_LESSEQ,NULL);
					}
					else
						set_token(token,E_invld,NULL);
				}
				else if ( is_divider(znak,operator_divider ) || znak == '?') // koli <? php
				{
					ungetc(current_pos);
					set_token(token,E_LESS,NULL);
				}
				else
					set_token(token,E_invld,NULL);								
				return;
			}							
			// --------------------------------------------------------
			
			case t_fraction:
			{
				if(znak == '/')
				{
					while(getc(current_pos) != '\n') {}; // preskoc vsetko az do konca riadku
					scanner_line++;
					next_state = INIT;
					break;
				}
				else if (znak == '*')	
				{
					Buffer->size--;	// pop
					next_state = t_block_c;
					break;
				}
				else if (is_divider(znak,operator_divider))
				{
					ungetc(current_pos);
					set_token(token,E_DIV,NULL);
					return;
				}
				else
				{
					set_token(token,E_invld,NULL);
					return;									
				}
			}
			// --------------------------------------------------------
			
			case t_concat:	
			{
				if (is_divider(znak,operator_divider))
					set_token(token,E_CONCAT,NULL);
				else
					set_token(token,E_invld,NULL);
				return;
			}
							
			// --------------------------------------------------------
							
			case t_exclam:	
			{	
				if (znak == '=' && getc(current_pos) == '=')
					set_token(token,E_not_eq,NULL);
				else
				{
					ungetc(current_pos);
					set_token(token,E_invld,NULL);
				}
				return;
			}
							
			// --------------------------------------------------------	

			case t_float:	
			{	//123.znak

				if(znak == 'e' || znak == 'E')
				{
					buffer_push(znak);
					next_state = t_exp;
					break;		
				}
				
				while(isdigit(znak))
				{
					buffer_push(znak);
					znak = getc(current_pos);	
				} 
				
				// 123.45+
				if(is_divider(znak,number_divider))
				{
					set_token(token,E_DOUBLE,Buffer->ptr);
					ungetc(current_pos);
					return;
				}
				else if (znak == 'e' || znak == 'E')
				{
					buffer_push(znak);
					next_state = t_exp;
					break;	
				}
				else
				{
					set_token(token,E_invld,NULL);	
					return;
				}							
			}
			// --------------------------------------------------------
			case t_exp:		
			{								
				// optional +/-
				if(znak == '+' || znak == '-') 
				{
					buffer_push(znak);
					znak = getc(current_pos);
				}
				
				// cisla :
				do
				{ 
					if(isdigit(znak))
					{
						buffer_push(znak);
						znak = getc(current_pos);	
					}
					else if(znak == 0) // ak by skor ako koniec cisla prisiel EOF
					{
						set_token(token,E_invld,NULL);
						ungetc(current_pos);
						return;
					}
					else 
						break;	
				} while(1);
				
				// koniec exponentu:							
				if(is_divider(znak,number_divider))
				{
					// 123.45E-23 123.45E23
					ungetc(current_pos);
					set_token(token,E_DOUBLE,Buffer->ptr);	
				}
				else // 123.5E-3ň
					set_token(token,E_invld,NULL);				
				return;				

			}
			// --------------------------------------------------------	
			
			case t_lit: 	
			{
				while(znak != '"')
				{
					if(znak == '\\')
					{
						next_state = t_escape;
						break;
					}
					
					if(znak == 0)
					{
						set_token(token,E_invld,NULL);
						ungetc(current_pos);
						return;
					}
					
					if(znak == '$') // dolar musi byt cez escape
					{
						set_token(token,E_invld,NULL);
						return;
					}
					buffer_push(znak);
					znak = getc(current_pos);
				}
				
				
				
				if(next_state == t_escape)
					break;
				else
				{
					buffer_push(znak); // ulozime "
					set_token(token,E_liter,Buffer->ptr);
					return;
				}
			}	
							
			// --------------------------------------------------------	
			case t_escape:	
			{
				if(znak < ' ') // < 31
				{
					set_token(token,E_invld,NULL);
					return;
				}
				
				char hexa_cislo[1];
				unsigned cislo;
				
				switch(znak)
				{
					case '"' :	
					case '$' :
					case '\\':
								buffer_push(znak);
								break;
								
					case 'x':	
								hexa_cislo[0] = getc(current_pos);
								if(hexa_cislo[0] != 0)
									hexa_cislo[1] = getc(current_pos);
								else
								{	// ak by na konci suboru bolo "\x tak by nas mohol navstivit ujo segfault
									set_token(token,E_EOF,NULL);
									return;
								}
								
								if(sscanf(hexa_cislo,"%x",&cislo) == 1)
								{
									buffer_push(cislo);
								}
								else
								{
									set_token(token,E_invld,NULL);
									return;
								}
								
								break;
								
					case 'n':	buffer_push(10); // \n
								break;
					
					case 't':	buffer_push(11); // \t
								break;
					
					
					default:	buffer_push('\\'); // escape sekvencia nemoze sposobit chybu
								buffer_push(znak); // cili to tam proste pushnem tak jak je a jeee
								break;
				}
				next_state = t_lit;
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

