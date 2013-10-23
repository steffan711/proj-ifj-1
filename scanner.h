   /** Globalne premenne **/
 char* current_pos; // aktualna pozicia scannera v subore, prosim neprepisovat
 unsigned scanner_line, scanner_column;
 unsigned short skip_mode; // (0 = nespi, 1 = spi do konca riadku, 2 = spi do konca blokoveho komentara)
 const char* file_origin;


 
typedef enum {
	E_EQ,			// =
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
	E_tripleeq,		// ===
	E_Lparentheses,	// (
	E_Rparentheses,	// )
	E_lBrack,		// [
	E_rBrack,		// ]
	E_laBrack,		// {
	E_raBrack,		// }
	E_invld,		// je to invalid
	E_EOF,			// padla
} TOKEN_TYPE;

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
	t_line_c,  	// riadkovy komentar
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
	int_divider,
	fract_divider,	
	float_divider,
} divider; // oznacenie mnoziny znakov ktora moze nasledovat za nejakym znakom

typedef struct _tStringBuffer	// nekonecny retazec [ak nepretecie velkost ... ]
{
	unsigned part_size;	// velkost jedneho bloku
    unsigned allocated_size; 
    unsigned size; // aktualny pocet znakov v retaci
    char *ptr;	// ukazatel na retazec
} tStringBuffer;
tStringBuffer* pointer;


 

 void scanner_init(char *file_start);
 T_token scanner_get_token();
 void print_token(T_token* token);