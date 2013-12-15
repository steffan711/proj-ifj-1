/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file ial.h - Hlavickovy subor modulu ial.c
 * @author Filip Ilavský, Oliver Nemček
 */

#ifndef _IAL_H
#define _IAL_H

#include "types.h"
#include <stdbool.h>

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

#include "generator.h"
void quicksort( char *input, int left, int right );
int kmpmatch( const char *text, int text_size, const char *pattern, int pattern_size );



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

#endif //_IAL_H