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
#include "debug.h"


/**
 *
 * Testovaci modul
 */
int main(void)
{
    
    if ( GeneratorInit() != E_OK )
    {
        printf("GeneratorInit() failed\n ");
        return 0;
    }
    
    T_token *ptr1 = calloc( 1, sizeof( T_token ) );
    T_token *ptr2 = calloc( 1, sizeof( T_token ) );
    T_token *ptr3;
    
    T_token assigntoken = { .ttype = T_VAR, .line = 0, .length = 3, .data._string = "ABC" };
    //$ABC = 10*5;
    ptr1->ttype = E_LITER;
    ptr1->data._string = "AB";
    ptr1->length = 2;
    
    ptr2->ttype = E_LITER;
    ptr2->data._string = "ABC";
    ptr2->length = 3;
    
    assign( &assigntoken );
    E_ERROR_TYPE retval = eval( ptr1, ptr2, E_LESS );
    
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    
    retval = eval( ptr1, NULL, E_TERM );
    if( retval != 0 )
        printf(" retval = %d\n", retval); 
    // $DEF = 5.5;
    ptr1 = calloc( 1, sizeof( T_token ) );
    assigntoken.data._string = "DEF";
    assign( &assigntoken );
    
    ptr1->ttype = E_DOUBLE;
    ptr1->data._double = 5.5;
    
    retval = eval(ptr1, NULL, E_TERM);
    
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    //$MEC = $DEF . $DEF;    
    assigntoken.data._string = "MEC";
    
    assign( &assigntoken );
    ptr1 = calloc( 1, sizeof( T_token ) );
    ptr2 = calloc( 1, sizeof( T_token ) );
    
    ptr2->ttype = E_VAR;
    ptr1->ttype = E_VAR;
    ptr2->data._string = "ABC";
    ptr1->data._string = "DEF";
    ptr2->length = 3;
    ptr1->length = 3;
    retval = eval(ptr1, ptr2, E_CONCAT);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    retval = eval(ptr1, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    PRINTD("vsetko je ok\n");
      ////////////////////////////////////////////  
    ptr1 = calloc( 1, sizeof( T_token ) );
    ptr2 = calloc( 1, sizeof( T_token ) );
    
    ptr3 = calloc( 1, sizeof( T_token ) );
    
        ptr1->ttype = E_INT;
    ptr1->data._int = 10;
    
    ptr2->ttype = E_INT;
    ptr2->data._int = 5;
    ptr3->ttype = E_IDENT;
    //ptr2->data._string = "ABC";
    //ptr1->data._string = "DEF";
    ptr3->data._string = "nut";
    ptr2->length = 3;
    ptr1->length = 3;
    ptr3->length = 3;
    
    assign( &assigntoken );
    T_token *pole[] = { ptr3, ptr1, ptr2 };
    retval = evalf(pole, 3);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    retval = eval(ptr3, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    /* skuska ifu */
    
    setstate(S_WHILE_BEGIN);
    ptr3 = calloc( 1, sizeof( T_token ) );
    ptr3->ttype = E_VAR;
    ptr3->length = 3;
    ptr3->data._string = "DEF";
    retval = eval(ptr3, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
        
    setstate(S_WHILE_END);
    //setstate(S_IF_END);
    assign(NULL);
    ptr3 = calloc( 1, sizeof( T_token ) );
    ptr3->ttype = E_VAR;
    ptr3->length = 3;
    ptr3->data._string = "DEF";
     retval = eval(ptr3, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    
 
    ptr3 = calloc( 1, sizeof( T_token ) );
    ptr3->data._string = "nut";
    ptr3->ttype = E_IDENT;
    ptr3->length = 3;
    define(ptr3);
    free(ptr3);
    T_token p1 = { .ttype = T_VAR, .line = 0, .length = 3, .data._string = "ABC" };
    T_token p2 = { .ttype = T_VAR, .line = 0, .length = 3, .data._string = "BCD" };
    T_token p3 = { .ttype = T_VAR, .line = 0, .length = 3, .data._string = "BCD" };
    addparam(&p1);
    //addparam(&p2);
    addparam(&p3);
    retval = addparam(NULL);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    assign(NULL);
    ptr1 = calloc( 1, sizeof( T_token ) );
    
    ptr1->ttype = E_DOUBLE;
    ptr1->data._double = 5.5;
    
    retval = eval(ptr1, NULL, E_TERM);
    setstate(S_FUNCTION_END);
    
    
    
     ptr1 = calloc( 1, sizeof( T_token ) );
    ptr2 = calloc( 1, sizeof( T_token ) );
    
    ptr3 = calloc( 1, sizeof( T_token ) );
    
    ptr1->ttype = E_INT;
    ptr1->data._int = 10;
    
    ptr2->ttype = E_INT;
    ptr2->data._int = 5;
    ptr3->ttype = E_IDENT;
    ptr3->data._string = "nut";
    ptr2->length = 3;
    ptr1->length = 3;
    ptr3->length = 3;
    
    assign( &assigntoken );
    pole[0] = ptr3;
    pole[1] = ptr1;
    pole[2] = ptr2;
    retval = evalf(pole, 3);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    retval = eval(ptr3, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    
    
    setstate(S_FILE_END);
    PrintTape( FT.tape ); 
    PRINTD("/**************************/\n");
    PrintTape( FT.btreeroot->metadata.tape ); 
    PRINTD("funkcia %s ma pocet parametrov %u je v stave %d a pointer na fixlist ma %p, nextptr %p\n", FT.btreeroot->metadata.name,FT.btreeroot->metadata.param_count, FT.btreeroot->metadata.state, (void*)FT.btreeroot->metadata.fix_list->instr, (void*)FT.btreeroot->metadata.fix_list->next );
    PRINTD("function tree %d\n",FT.unknown_count);
    /*printf("-------------\n");
    AddBuiltinFunction("sracka", 6, 6, 0, F_DUMMY);
    printf("-------------\n");
    AddBuiltinFunction("hadanka", 7, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    AddBuiltinFunction("hazzzz", 6, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    AddBuiltinFunction("hazzzzz", 7, 6, 0, F_PUTSTRING);
    printf("-------------\n");
    struct metadata_function *ptr;
    LookupFunction("fibonacci", 9, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
    LookupFunction("tamagoci", 8, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
    LookupFunction("abs", 3, &ptr);
    printf("aha, mam funkciu %s a je typu %d \n", ptr->name, ptr->state);
 
    struct metadata_var* p;
    
    BTlookup(STableGlobal, "premetna", 8, &p);
    printf("premenna %s dostala cislo %d\n", p->name, p->offset);
    BTlookup(STableGlobal, "stra\0snedlhe pomenovanie niecoho zaujimaveho", 44, &p);
    BTfind(STableGlobal, "stra\0snedlhe pomenovanie niecoho zaujimaveho", 44, &p);
    printf("premenna ");
    for (unsigned int i = 0; i< p->name_size; i++)
        putchar(p->name[i]);
    printf("dostala cislo %d\n", p->offset);
    */
    GeneratorErrorCleanup();
    return 0;
}