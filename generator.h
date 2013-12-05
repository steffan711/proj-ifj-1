/**
 * @file generator.h
 *
 * @brief Definicie datovych typov a rozhranie modulu genertor kodu
 * @author olii
 */

#ifndef _GETERATOR_H
#define _GETERATOR_H


#include <stdbool.h>
#include "scanner.h"
#include "types.h"

/* ********************************** */
/* ******** Tabulka symbolov ******** */
/* ********************************** */
/**
 *  struct metadata_var - obsahuje metadata o symbole
 *  *name - meno symbolu, kluc do tabulky
 *  name_size - dlzka retazca
 *  offset - pozicia premennej v tabulke parametrov a lokalnych premennych
 */
struct metadata_var {
    char *name;                     // KEY
    unsigned int name_size;         // dlzka retazca name
    unsigned int offset;            // poradie v taluble parametrov a lokalnych premennych
    bool assigned;            
};

/**
 *  struct var_item - uzol binarneho stromu
 *  *lptr - ukazovatel na lavy podstrom
 *  *rptr - ukazovatel na pravy podstrom
 */
struct var_item {
    struct metadata_var metadata;   // metadata o premennej
    struct var_item* lptr;
    struct var_item* rptr;
};

/**
 *  struct sym_tree_handle - deskriptor binarneho stromu tabulky symbolov
 *  counter - pocitadlo jednoznacneho identifiktoru premennej
 */
struct sym_tree_handle {
    unsigned int counter;
    struct var_item *btreeroot; // korenovy uzol stromu
};

typedef struct sym_tree_handle STable;
typedef struct var_item STableNode;
typedef struct metadata_var STableData;

/* ******** Tabulka symbolov ******** */
/* ********        END       ******** */
/* ********************************** */


/* ********************************** */
/* ************ Instrukcia ********** */
/* ********************************** */

enum vartype { VAR_UNDEF = 0, VAR_INT, VAR_BOOL, VAR_DOUBLE, VAR_NULL,  VAR_LOCAL, VAR_NO_VAR, VAR_STRING, VAR_CONSTSTRING };

typedef struct variable
{
    enum vartype type;
    union
    {
        int _int;
        char* _string;
        bool _bool;
        double _double;
        unsigned int offset;
        struct variable *prev_local;
        struct instruction* EIP;
    } data;
    unsigned int size;
} T_DVAR;

enum opcodes { DUMMY = 0, START, CREATE, MOV, RET, PUSH, COND, JMP,
              CALL, CALL_BUILTIN,
              INC, DEC,
              PLUS, MINUS,
              CONCAT, AND, OR, EQUAL, NONEQUAL, DIV, MUL, LESS, GREATER, LESSEQ, GREATEREQ,
              };

struct instruction {
    enum opcodes opcode;
    union
    {
        unsigned int size;
        struct
        {
            E_ERROR_TYPE (*func)( T_DVAR[], int, T_DVAR *);
            unsigned int dest;
        } builtin;
        struct
        {
            struct instruction *jmp;
            unsigned int dest;
            T_DVAR op1;
        } jump;
        struct 
        {
            unsigned int dest;
            T_DVAR op1;
            T_DVAR op2;
        } tac; // Three Adress Code
    } attr;
    struct instruction *next;
};

struct instruction_list {
    struct instruction *instr; // pointer na instrukciu
    struct instruction_list *next; // nasledujuci pointer
};

typedef struct instruction Instruction;
typedef enum opcodes Opcode;
typedef struct instruction_list InstructionList;

/* ************ Instrukcia ********** */
/* ********        END       ******** */
/* ********************************** */

/* ********************************** */
/* ******** Tabulka funkcii ********* */
/* ********************************** */

enum builtin_functions {F_DUMMY, F_PUTSTRING}; // TODO: FILIP

/**
 *  function_state - oznacuje v akom stave sa funkcia nachadza
 *  E_DEFINED - pozname pocet parametrov
 *  E_UNKNOWN - pocet parametrov je neznamy
 *  E_BUILTIN - zabudovana funkcia
 *  E_DUMMY - placeholder
 */
enum function_state { E_DEFINED, E_UNKNOWN, E_BUILTIN, E_DUMMY }; //TODO: DUMMY

/**
 *  struct metadata_function - obsahuje metadata o funkcii, prevazne semanticka analyza
 *  *name - meno funkcie
 *  name_size - dlzka retazca *name
 *  state - stav v akom sa funkcia nachadza
 *  param_count - pocet parametrov +- stav E_DEFINED = skutocny pocet parametrov
 *                                  ∟ stav E_UNKNOWN = minimalny najdeny pocet parametrov
 *  frame_count - pocet vsetkych lokalnych premennych a parametrov
 *  unlimited_param - zabudovana funkcia moze mat neobmedzene vela parametrov
 *  builtin_id - identifikator zabudovanej funkcie
 *  *fix_list - zoznam, kde treba opravit volania tejto funkcie
 *  *tape - ukazuje na prvu instrukciu funkcie
 */
struct metadata_function {          // informacie o funkcii
    char *name;                     // KEY
    unsigned int name_size;         // dlzka mena funkcie
    enum function_state state;      // stav
    unsigned int param_count;       // skutocny pocet
    unsigned int frame_count;       // velkost ramca premennych
    bool unlimited_param;           // funkcia bere neobmedzeny pocet parametrov - napr putchar
    E_ERROR_TYPE (*builtin_id)( T_DVAR[], int, T_DVAR *); // enum zabudovanej funkcie
    struct instruction_list *fix_list; 
    unsigned int first_line;        // prva referencia na funkciu, riadok
    struct instruction *tape;       // tape
};

/**
 *  struct f_item - uzol binarneho stromu
 */
struct f_item {
    struct metadata_function metadata; // atribut
    struct f_item* lptr;
    struct f_item* rptr;
};

/**
 *  struct function_tree_handle - deskriptor binarneho stromu tabulky funkcii
 *  count - pocet funkcii vsetkych
 *  unknown_count - pocet nedefinovanych funkcii
 *  *btreeroot - koren binarneho stromu
 *  *tape - instrukcna paska hlavneho programu
 */
struct function_tree_handle {
    unsigned count;
    unsigned unknown_count;
    struct f_item *btreeroot;       // koren
    struct instruction *tape;       // ukazovatel na hlavnu pasku
};

typedef struct function_tree_handle FTable;
typedef struct f_item FTableNode;
typedef struct metadata_function FTableData;

/* ******** Tabulka funkcii ********* */
/* ********        END       ******** */
/* ********************************** */

/* ********************************** */
/* ************ Zasobniky *********** */
/* ********************************** */

/**
 *  struct map_table - mapovacia tabulka s priamym pristupom
 *  size; - velkost pola map
 *  used_space - pouzite miesto v tabulke, nieco ako vrchol zasobnika
 *  map - flexibilne pole
 */
struct map_table {
    unsigned int size;
    unsigned int used_space;
    int map[];
};

/**
 *  struct ptr_stack - zasobnik pointrov, vyuzivane pri skokovych instrukciach
 *  size - velkost pola stack
 *  top - vrch zasobnika
 *  stack - flexibilne pole ukazovatelov na instrukcie
 */
struct ptr_stack {
    int size;
    int top;
    Instruction* stack[];
};

typedef struct map_table MapTable;
typedef struct ptr_stack PtrStack;

/* ************ Zasobniky *********** */
/* ********        END       ******** */
/* ********************************** */

struct InstructionTapeBuffer
{
    unsigned int size;
    Instruction* array[];
};

/* Nastavovanie stavu generatora */
enum gen_state { S_DEFAULT = 0, S_IF_BEGIN, S_IF_ELSE, S_IF_END,
                 S_WHILE_BEGIN, S_WHILE_END, S_FUNCTION_END, S_FILE_END };

/* Definicie globalnych premennych */
extern const int FLEXIBLE_ARRAY_MEMBER; // pociatocna velkost zasobnika
extern FTable FT;                       // tabulka funkcii
extern STable *STableLocal;             // lokalna tabulka symbolov
extern STable *STableGlobal;            // globalna tabulka symbolov
extern STable *SwitchSTable;            // vyhybka pre tabulku symbolov
extern PtrStack *ptrstack;              // zasobnik pointrov, kvoli opravam na paske
extern MapTable *Localmap;              // lokalne mapovanie premennych
extern MapTable *Globalmap;             // mapovanie premennych pre hlavne telo programu
extern MapTable *SwitchMap;             // vyhybka na mapovanie
extern Instruction *GlobalTape;         // ukazovatel na poslednu instrukciu hlavneho programu
extern Instruction *LocalTape;          // ukazovatel na poslednu instrukciu funkcie
extern Instruction *SwitchTape;         // vyhybka ukazovatela
extern enum gen_state State;            // uchovava stav generatora, ine chovanie niektorych funkcii
extern FTableData *actualfunction;      // aktualna funkcia
extern FTableData *callfunction;        // docasne uchovanie volanej funkcie
extern STableData *assignvar;           // docasne uchovanie premennej do ktorej sa priraduje


E_ERROR_TYPE AddBuiltinFunction( char *name,
                                 unsigned int size,
                                 unsigned int param_count,
                                 bool unlimited,
                                 E_ERROR_TYPE (*builtin_id)( T_DVAR[], int, T_DVAR *)
                                );
                                
E_ERROR_TYPE LookupFunction(char *name, unsigned int size, unsigned int line, FTableData **ptr_out);

void DeleteFT(void);    

void BTinit( STable  *tree );

E_ERROR_TYPE BTfind( STable *tree,
                     char *name,
                     int name_size,
                     STableData** ptr_out
                    );
                    
E_ERROR_TYPE BTlookup( STable *tree,
                       char *name,
                       int name_size,
                       STableData **ptr_out,
                       bool *added  
                      );
                      
void DeleteBT( STable *tree );
void FindUnknownFunctions(FTableNode *ptr);
E_ERROR_TYPE PtrStackInit(PtrStack **ptr);
E_ERROR_TYPE PtrStackCheck(PtrStack **ptr);
E_ERROR_TYPE MapTableInit(MapTable **ptr);
E_ERROR_TYPE MapTableCheck(MapTable **ptr);
int lexsstrcmp( const char * str1, const char * str2, int str1_size, int str2_size );


void PrintTape( Instruction *ptr );

void GeneratorDeleteTapes(struct InstructionTapeBuffer *ptr);
E_ERROR_TYPE GeneratorPrepareTape(struct InstructionTapeBuffer **ptr);
void GeneratorErrorCleanup(void);
E_ERROR_TYPE GeneratorInit();
E_ERROR_TYPE define(T_token *token);
E_ERROR_TYPE addparam(T_token *token);
E_ERROR_TYPE setstate(enum gen_state state);
E_ERROR_TYPE assign(T_token *op1);
E_ERROR_TYPE eval(T_token *op1, T_token *op2, TOKEN_TYPE operation);
E_ERROR_TYPE evalf(T_token *array[], unsigned int size);

#endif