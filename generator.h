/**
 * @file generator.h
 *
 * @brief Definicie datovych typov a rozhranie modulu genertor kodu
 * @author olii
 */

#ifndef _GETERATOR_H
#define _GETERATOR_H


#include <stdbool.h>

#include "types.h"



extern const int FLEXIBLE_ARRAY_MEMBER; // velkost zasobnika

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
    bool unlimited_param;           // funkcia bere neobmedzeny pocet parametrov - napr putchar
    enum builtin_functions builtin_id; // enum zabudovanej funkcie
    struct instruction_list *fix_list; 
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
 *  unknown_count - pocet nedefinovanych funkcii
 *  *btreeroot - koren binarneho stromu
 *  *tape - instrukcna paska hlavneho programu
 */
struct function_tree_handle {
    int unknown_count;
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
/* ************ Instrukcia ********** */
/* ********************************** */

struct instruction {
    int dummy;
    struct instruction *next;
};

struct instruction_list {
    struct instruction *instr; // pointer na instrukciu
    struct instruction_list *next; // nasledujuci pointer
};

typedef struct instruction Instruction;
typedef struct instruction_list InstructionList;

/* ************ Instrukcia ********** */
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
    int size;
    int used_space;
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


extern FTable FT;

E_ERROR_TYPE AddBuiltinFunction( char *name,
                                 unsigned int size,
                                 unsigned int param_count,
                                 bool unlimited,
                                 enum builtin_functions builtin_id
                                );
                                
E_ERROR_TYPE LookupFunction(char *name, unsigned int size,  FTableData **ptr_out);
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
                       STableData **ptr_out  
                      );
void DeleteBT( STable *tree );
E_ERROR_TYPE PtrStackInit(PtrStack **ptr);
E_ERROR_TYPE PtrStackCheck(PtrStack **ptr);
E_ERROR_TYPE MapTableInit(MapTable **ptr);
E_ERROR_TYPE MapTableCheck(MapTable **ptr);

#endif