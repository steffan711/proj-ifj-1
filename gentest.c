/**
 * @file test.c
 *
 * @brief Hlavna vetva
 * @author Vlado
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"
#include "scanner.h"
#include "generator.h"


/**
 *
 * Testovaci modul
 */
int main( )
{
    
    struct ptr_stack *ptrstack;
    PtrStackInit( &ptrstack );
    for(int i = 0; i<FLEXIBLE_ARRAY_MEMBER; i++)
        ptrstack->stack[i] = NULL;
    free(ptrstack);
    
    struct map_table *map;
    MapTableInit( &map );
    for(int i = 0; i<FLEXIBLE_ARRAY_MEMBER; i++)
        map->map[i] = 0;
    free(map);
    
    printf("-------------\n");
    AddBuiltinFunction("sracka", 6, 6, 0, F_DUMMY);
    printf("-------------\n");
    AddBuiltinFunction("hadanka", 7, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    AddBuiltinFunction("hazzzz", 6, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    AddBuiltinFunction("hazzzzz", 7, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    printf("koren %.6s\n", FT.btreeroot->metadata.name);
    printf("left %.7s\n", FT.btreeroot->lptr->metadata.name);
    printf("right %.6s\n", FT.btreeroot->rptr->metadata.name);
    printf("left left  %.7s\n", FT.btreeroot->lptr->lptr->metadata.name);
    struct metadata_function *ptr;
    LookupFunction("fibonacci", 9, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
    LookupFunction("tamagoci", 8, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
    LookupFunction("abs", 3, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
    DeleteFT();
    
    struct sym_tree_handle a;
    BTinit(&a);
    struct metadata_var* p;
    
    BTlookup(&a, "premetna", 8, &p);
    printf("premenna %s dostala cislo %d\n", p->name, p->offset);
    BTlookup(&a, "stra\0snedlhe pomenovanie niecoho zaujimaveho", 44, &p);
    BTfind(&a, "stra\0snedlhe pomenovanie niecoho zaujimaveho", 44, &p);
    printf("premenna ");
    for (unsigned int i = 0; i< p->name_size; i++)
        putchar(p->name[i]);
    printf("dostala cislo %d\n", p->offset);
    DeleteBT(&a);
    return 0;
}