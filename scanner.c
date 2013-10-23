/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xmarti62, xnemce03, xilavs01
 *
 * Subor: scanner.c - modul lexikalniho analyzatoru
 * Autor: Vladimír Čillo, xcillo00
 */


#include <stdio.h>
#include <stdlib.h>

typedef enum {
	E_EQ = 15,		// aby nedoslo ku konfliktu s enumom navratovych kodov
	E_LESS,			// <
	E_GREATER,		// >
	E_LESSEQ,		// <=
	E_GREATEREQ,	// >=
	E_PLUS,			// +
	E_MINUS,		// -
	E_MULT,			// *
	E_DIV,			// /
	E_SEMICL,		// ;
	E_COMA,			// ,
	E_CONCAT,		// .
	E_IDENT,		// identifikator
	E_VAR,			// premenna
	E_INT,			// integer
	E_DOUBLE,		// double
	E_STRING,		// literal
	E_tripleeq;		// ==
	E_Lparentheses,	// (
	E_Rparentheses,	// )
	E_lBrack,		// [
	E_rBrack,		// ]
	E_laBrack,		// {
	E_laBrack,		// }
	E_INVALID,		// je to invalid
	// ** TODO ** Klujcove slova co ? Nezasluzia si vlastnu tabulku ?
	// E_ELSE,
	// E_FUNC,
	// E_IF,
	// E_RET,
	// E_WHILE,
	// E_FALSE,
	// E_NULL,
	// E_TRUE,
	// E_EOF
} TOKEN_TYPE;

typedef struct token {
	TOKEN_TYPE ttype;
	unsigned line;
	unsigned column;
	void *data;
} S_token;

typedef struct _tStringBuffer	// nekonecny retazec [ak nepretecie velkost ... ]
        {
		  unsigned part_size;	// velkost jedneho bloku
          unsigned allocated_size; 
          unsigned size; // aktualny pocet znakov v retaci
          char *ptr;	// ukazatel na retazec
        } tStringBuffer;



/**
 * @function GimmeString
 * 
 * @param velkost v B, po kolkych sa bude retazec zvacsovat
 * @return stringBuffer 
 */		
tStringBuffer* gimme_string(int size) // vytvori abstrakciu nekonecneho retazca
{
	tStringBuffer* tmp = malloc(sizeof(tStringBuffer));
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
 * @return E_ERROR_TYPE
 */	

int append_string(tStringBuffer* string, char znak) // doplni znak na koniec retazca
{
	if(string == NULL)
	{   // TODO : funkcia ktora osetri chybu
		return E_OTHER;
	}
	else if (string-> allocated_size > string-> size ) // este sa donho zmesti
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
			return E_OTHER;
		}
		string->ptr[string->size++] = znak;
	}
	return E_OK;
}

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


 /** Globalne premenne **/
 char* current_pos; // aktualna pozicia scannera v subore, prosim neprepisovat

 
 
typedef enum {
	INIT,	
	FINISH,		// EOF
	t_id,		// identifikator
	t_var,		// premenna
	t_int,		// integer
	t_ass,		// = 
	t_comp,		// porovnanie ==
	t_inv,		// nieco strasne
	t_fraction,	// /
	t_block_c, 	//blokovy komentar
	t_line_c,  	//riadkovy komentar
	
} FSM_STATE; 
 
S_token scanner(char *data)
{

	current_pos = data;
	FSM_STATE next_state = INIT;
	int znak;
	unsigned line,column = 0;
	
	S_token next_token;
	next_token.ttype = t_inv;
	next_token.line = 0;
	next_token.column = 0;
	next_token.data = NULL;
	
	while ( next_state != FINISH )
	{	
		switch(next_state)
		{	
			case INIT:		{
								znak=getc(current_pos);
								if(isalpha(znak) || znak == '_') // A-Za-z_
									next_state = t_id;
								else if(isdigit(znak))
								{
									next_state = t_int;
								}
								else if (znak < ' ')	// znaky s ASCII hodnotou mensiou ako 32 nie su  sucastou jazyka
								{
									next_state = t_inv;
								}
								else
								{
									switch(znak)
									{	
										case '$':	{	next_state = t_var;
														break;
													}			
										case '=':	{
														next_state = t_ass;
													}
										case '/':	{
														next_state = t_comm;
													}
										case 0:		{
														next_state = FINISH;
													}										
													
										default: 	{	
														next_token.ttype = E_INVALID;
														next_token.data = NULL;
														next_state = t_inv;
														break;
													}
									}
								}
							}
							
			case t_var:		{	
								next_state = t_id;
								return next_token;
							}			
			case t_inv:		{
								next_token.ttype = 
								return next_token;
							}
			case t_id:		{
								while(isalnum(getc(current_pos)))
								{
									
								}
								return next_token;
							}
			case t_int:		{
								break;
							}
			case t_ass:		{	// =
								switch(getc(current_pos))
								{
									case '=' : 	{ 	znak = getc(current_pos);
													if(znak == '=' && isspace(getc(current_pos))) // === 
														next_token.ttype = E_tripleeq;
														next_token.line = line;
														next_token.column = column;
														next_token.data= NULL;
														return next_token;
													break;
												}
									default:	break;
								}
								

							}
			case t_block_c:	{
								break;
							}			
			case t_line_c:	{
								break;
							}									
			case FINISH:	break;
		}
	}
	return next_token;
}

