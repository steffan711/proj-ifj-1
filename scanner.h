/*
 * Projekt: IFJ13
 * Riesitelia: xcillo00, xmarti62, xnemce03, xilavs01
 *
 * Subor: scanner.h
 */

 #define allocation_coeficient 2
 
/** Typy tokenov **/
typedef enum {
	E_EQ = 1,		// =
	E_COMP,			// ==
	E_tripleeq,		// ===
	E_not_eq,		// !==
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
	E_Lparentheses,	// (
	E_Rparentheses,	// )
	E_lBrack,		// [
	E_rBrack,		// ]
	E_laBrack,		// {
	E_raBrack,		// }
	E_invld,		// je to invalid
	E_EOF,			// padla
} TOKEN_TYPE;

/** Stavy konecnej masinky **/
typedef enum {	
	INIT,	
	FINISH,		// EOF
	t_id,		// identifikator
	t_int,		// integer
	t_float,
	t_ass,		// = 
	t_comp,		// porovnanie ==
	t_fraction,	// /
	t_block_c, 	// blokovy komentar
	t_add,		// +
	t_sub,		// -
	t_star,		// *
	t_concat,	// .
	t_greater,	// >
	t_less,		// <
	t_exclam,	// !	
} FSM_STATE;


typedef struct token {
	TOKEN_TYPE ttype;
	unsigned line;
	unsigned column;
	void *data; 
} T_token;

typedef enum 
{
	operator_divider,
	number_divider,
} divider; // oznacenie mnoziny znakov ktora moze nasledovat za nejakym znakom

/** Bezodny IKEA buffer **/
typedef struct _tStringBuffer	
{
    unsigned allocated_size; 
    unsigned size; 	// aktualny pocet znakov v retaci, ptr[size] = '\0'
    char *ptr;		// ukazatel na retazec
} tStringBuffer;

 /** Globalne premenne **/
 char* 			current_pos; // aktualna pozicia scannera v subore, prosim neprepisovat
 unsigned 		scanner_line, scanner_column;
 const char* 	file_origin;
 tStringBuffer 	stack; // nekonecny zasobnik znakov
 tStringBuffer*	Buffer;
 
/** Funkcie **/
int buffer_push(char znak);
int scanner_init(char *file_start);
void scanner_shutdown();
void scanner_get_token(T_token* token);
void print_token(T_token* token);
