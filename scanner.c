/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xmarti62, xnemce03, xilavs01
 *
 * Subor: scanner.c - modul lexikalniho analyzatoru
 * Autor: Vladimír Čillo, xcillo00
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"
#include "scanner.h"

		
		

/**
 * @function GimmeString
 * 
 * @param velkost v B, po kolkych sa bude retazec zvacsovat
 * @return stringBuffer / NULL pri neuspechu
 */		
tStringBuffer* gimme_string(int size) // vytvori abstrakciu nekonecneho retazca
{
	tStringBuffer* tmp = malloc(sizeof(tStringBuffer));
	if(tmp == NULL)
		return NULL;
	tmp->part_size = size;
	tmp->allocated_size = 0;
	tmp->size = 0;
	tmp->ptr = NULL;
	return tmp; 
}

/**
 * @function append_string
 * 
 * @param1 ukazatel na nekonecny retazec
 * @param2 znak ktory treba doplnit
 * @return E_OK / E_OTHER
 */	

int append_string(tStringBuffer* string, char znak) // doplni znak na koniec retazca
{

	if (string-> allocated_size > string-> size ) // este sa donho zmesti
	{
		string->ptr[string->size++] = znak;
	}
	else // uz sa nezmesti do koze
	{
		string->allocated_size += string->part_size; // zvacsime mu velkost o jednu cast
		string->ptr = realloc(string->ptr,(string->allocated_size)*sizeof(char));
		if(string->ptr == NULL)
		{
			// TODO : funkcia ktora osetri chybu
			printf("\n Realloc fail !");
			exit(1);
			return E_OTHER;
		}
		string->ptr[string->size++] = znak;
	}
	return E_OK;
}


/**
 * @function free_string
 * @
 * @param1 ukazatel na nekonecny retazec
 * @return E_OK / E_OTHER
 */	

int free_string(tStringBuffer* string)
{
	if(string->ptr == NULL)
	{
		return E_OTHER;
	}
	free(string->ptr);
	free(string);
	return E_OK;
}

/**
 * @function set_token
 * Inicializuje strukturu tokenu potrebnymi datami
 * @param1 ukazatel na token
 * @param2 token type
 * @param3 ukazatel (niekde)
 * @return void
 */

static inline void set_token(T_token* ptr, TOKEN_TYPE type, void* data)
{
	ptr->ttype = type;
	ptr->line = scanner_line;
	ptr->column = scanner_column;
	ptr->data = data;
}
 
 static inline bool is_divider(char znak, int switcher) 
 {
	switch (switcher)
	{
		case number_divider: 	return ( isspace(znak) || znak == '/' || znak == '+' || znak == '-' || znak == '*' || znak == ';' || znak == ',') ? true : false;
		case operator_divider:	return ( isspace(znak) || znak == '$' || isdigit(znak) || znak == '?') ? true : false;
		default: return false;
	}
	return false;
 }
 // test: /* /a /7 /$
 
 /**
 * @function scanner_init
 * @
 * @param ukazatel na subor
 * @return -
 */	
 
 void scanner_init(char *file_start)
 {
	current_pos = file_start;
	scanner_line = 1;
	scanner_column = 0;
	file_origin = file_start;
 }
 
 void print_token(T_token* token)
 {
	printf("\nToken -> column: %u",token->column);
	printf("\nToken -> line: %u",token->line);
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
		case 28:	printf("\nToken -> ttype: E_EOF\n");		break;


		default: break;
	}
	printf("\nToken -> data: %p",token->data);
	
 }
 
 
 

T_token scanner_get_token()
{

	
	FSM_STATE next_state = INIT;
	int znak;
	
	T_token next_token;
	T_token* next = &next_token;
	set_token(next,E_invld,NULL); // inicializacia
	
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
		printf("%c",znak); // debug
		switch(next_state)
		{	
			// --------------------------------------------------------	
			case INIT:	
			{
				if(isalpha(znak) || znak == '_') // A-Za-z_
				{
					next_state = t_id;
					set_token(next,E_IDENT,NULL);
				}
				else if(isdigit(znak))
				{
					next_state = t_int;
				}
				else
				{
					switch(znak)
					{		
						case '/':	{
										next_state = t_fraction;
										break;
									}
						case '$':	{	next_state = t_id; //
										next->ttype= E_VAR;
										break;
									}			
						case '=':	{
										next_state = t_ass;
										break;
									}
						case '+':	{
										next_state = t_add;
										break;
									}
						case '-':	{
										next_state = t_sub;
										break;
									}	
						case '*':	{
										next_state = t_star; 
										break;
									}
						case '<':	{
										next_state = t_less;
										break;
									}
						case '>':	{
										next_state = t_greater;
										break;
									}
						case '"':	{
										break;
									}
						case '.':	{	
										next_state = t_concat;
										break;
									}
						case '(':	{
										set_token(next,E_Lparentheses,NULL);
										break;
									}
						case ')':	{
										set_token(next,E_Rparentheses,NULL);
										break;
									}
						case '!':	{
										next_state = t_exclam;
										break;
									}

						case 0:		{
										set_token(next,E_EOF,NULL);
										next_state = FINISH;
										break;
									}				
						default: 	{	
										set_token(next,E_invld,NULL); // znaky s ASCII hodnotou mensiou ako 32 nie su  sucastou jazyka
										return *next;
									}
					}	// switch
				}	// else
				break;
			} // while 
			
			// --------------------------------------------------------						
			case t_id:		
			{
				while(isalnum(znak) || znak == '_')
				{	
					printf("%c",znak);
					scanner_column++;
					znak = getc(current_pos);
						// debug					
				}
				ungetc(current_pos);
				return *next;
			}
			
			// --------------------------------------------------------	
			case t_int:		
			{	
				while(isdigit(znak))
				{
					// TODO: ulozit niekde
					znak = getc(current_pos);
					printf("t_int %c",znak);
				}
				if(znak == '.')
				{
					next_state = t_float;
					break;
				}
				else if (isspace(znak) || znak == ';' || znak == ',' ) // vlaidny oddelovac integera
				{
					ungetc(current_pos);
					set_token(next,E_INT,NULL);
					return *next;
				}
				else
				{
					set_token(next,E_invld,NULL);
					return *next;
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
							set_token(next,E_tripleeq,NULL);
						}
						else
							set_token(next,E_invld,NULL); // ====
					}
					else if (is_divider(znak,operator_divider))
					{
						set_token(next,E_COMP,NULL);
					}
					return *next;
				}
				else if(is_divider(znak,operator_divider))
				{
					set_token(next,E_EQ,NULL);
				}
				return *next;

			}
			
			// --------------------------------------------------------
			
			case t_block_c:	{	
								znak = getc(current_pos);
								while( znak != '*' ) 
								{
									if( znak == 0 )
									{
										next_state = FINISH;
										set_token(next,E_EOF,NULL);
										return *next;
									}
								}
								next_state = INIT;
								break;
							}	
													
			// --------------------------------------------------------				
							
			case t_greater:	{ // >
								if(znak == '=')
								{ // >=?
									if ( is_divider(getc(current_pos),operator_divider ) )
										set_token(next,E_GREATEREQ,NULL);
									else
										set_token(next,E_invld,NULL);
								}
								else if ( is_divider(znak,operator_divider) )
									set_token(next,E_GREATER,NULL);
								else
									set_token(next,E_invld,NULL);
									
								return *next;
							}	
			// --------------------------------------------------------
							
			case t_less:	{
								if(znak == '=')
								{ // <=?
									if ( is_divider(getc(current_pos),operator_divider ) )
										set_token(next,E_LESSEQ,NULL);
									else
										set_token(next,E_invld,NULL);
								}
								else if ( is_divider(znak,operator_divider ) )// <?
									set_token(next,E_LESS,NULL);
								else
									set_token(next,E_invld,NULL);								
								return *next;
							}							
			// --------------------------------------------------------
							
			case t_fraction:{
								if(znak == '/')
								{
									while(getc(current_pos) != '\n') {}; // preskoc vsetko az do konca riadku
									next_state = INIT;
									break;
								}
								else if (znak == '*')	
								{
									next_state = t_block_c;
									break;
								}
								else if (is_divider(znak,operator_divider))
								{
									set_token(next,E_DIV,NULL);
									return *next;
								}
								else
								{
									set_token(next,E_invld,NULL);
									return *next;									
								}
							}
			// --------------------------------------------------------
			
			case t_concat:	{
								if (is_divider(znak,operator_divider))
									set_token(next,E_CONCAT,NULL);
								else
									set_token(next,E_invld,NULL);
								return *next;
							}
			
			// --------------------------------------------------------			
							
			case t_comp:	{	
								next_state = FINISH;
								break;
							}
			
			// --------------------------------------------------------
			
			case t_add:		{
								if (is_divider(znak,operator_divider))
									set_token(next,E_PLUS,NULL);
								else
									set_token(next,E_invld,NULL);
								return *next;
							}

			// --------------------------------------------------------
							
			case t_sub:		{
								if (is_divider(znak,operator_divider))
									set_token(next,E_MINUS,NULL);
								else
									set_token(next,E_invld,NULL);
								return *next;
							}	
			
			// --------------------------------------------------------			
			
			case t_star:	{
								
								if (is_divider(znak,operator_divider))
									set_token(next,E_MULT,NULL);
								else
									set_token(next,E_invld,NULL);
								return *next;
							}
							
			// --------------------------------------------------------
							
			case t_exclam:	{	// otestovat !!!

								if (znak == '=' && getc(current_pos) == '=' && is_divider(getc(current_pos),operator_divider) )
									set_token(next,E_not_eq,NULL);
								else
									set_token(next,E_invld,NULL);
								return *next;
							}
							
			// --------------------------------------------------------	

			case t_float:	{
								next_state = FINISH;
								break;
							}
							
			case FINISH:	{	
								next_state = FINISH;
								break;
							}
		}
	}
	
	return *next;
}

