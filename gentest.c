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
    
    T_token assign = { .ttype = T_VAR, .line = 0, .length = 3, .data._string = "ABC" };
    //$ABC = 10*5;
    ptr1->ttype = E_INT;
    ptr1->data._int = 10;
    
    ptr2->ttype = E_INT;
    ptr2->data._int = 5;
    
    assing( &assign );
    E_ERROR_TYPE retval = eval( ptr1, ptr2, E_MULT );
    
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    print_token( ptr1 );
    
    retval = eval( ptr1, NULL, E_TERM );
    if( retval != 0 )
        printf(" retval = %d\n", retval); 
    // $DEF = 5.5;
    ptr1 = calloc( 1, sizeof( T_token ) );
    assign.data._string = "DEF";
    assing( &assign );
    
    ptr1->ttype = E_DOUBLE;
    ptr1->data._double = 5.5;
    
    retval = eval(ptr1, NULL, E_TERM);
    
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    //$MEC = $DEF + $DEF;    
    assign.data._string = "MEC";
    
    assing( &assign );
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
    print_token( ptr1 );
    retval = eval(ptr1, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    printf("vsetko je ok\n");
      ////////////////////////////////////////////  
    /*ptr1 = calloc( 1, sizeof( T_token ) );
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
    
    assing( &assign );
    T_token *pole[] = { ptr3, ptr1, ptr2 };
    retval = evalf(pole, 3);
    if( retval != 0 )
        printf(" retval = %d\n", retval);
    
    retval = eval(ptr3, NULL, E_TERM);
    if( retval != 0 )
        printf(" retval = %d\n", retval);*/
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
    setstate(S_FILE_END);
    PrintTape( FT.tape );  
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