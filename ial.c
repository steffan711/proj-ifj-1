/*
 * Projekt: IFJ13 2013
 * Riesitelia:  Vladimír Čillo (xcillo00)
 *              Oliver Nemček (xnemcek03)
 *              Štefan Martiček (xmarti62)
 *              Filip Ilavský (xilavsk01)
 * 
 * @file ial.c - Implementacia tabulky symbolov, Quick sort, Knuth-Morris-Prattov algoritmus
 * @author Filip Ilavský, Oliver Nemček
 */


#include "ial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "generator.h"
#include "debug.h"

/**
 * Pomocna funkcia, implementacia quicksortu
 *
 * @param retazec
 * @param leva medz
 * @param prava medz
 * @return void
 */
void quicksort( char *input, int beg, int end )
{
    int i = beg, j = end;
    char pom, PM = input[( beg + end ) / 2];
    
    do
    {
        while( input[i] < PM )
            i++;
        while( input[j] > PM )
            j--;
        if( i <= j )
        {
            pom = input[i];
            input[i] = input[j];
            input[j] = pom;
            i++;
            j--;
        }
        
    } while( i <= j );
    
    if( beg < j ) quicksort( input, beg, j );
    if( i < end ) quicksort( input, i, end );
}


/**
 * Pomocna funkcia, implementacia Knuth Morris Prattovho algoritmu
 *
 * @param retazec v ktorom sa hlada
 * @param hladany retazec
 * @return pozicia, -1 -> nenajdene, ina chyba nemoze nastat
 */
int kmpmatch( const char *text, int text_size, const char *pattern, int pattern_size )
{
	if( pattern_size == 0 )
        return 0;

	int m = 0, i = 0, Fail[pattern_size + 1], pos = 2, cnd = 0;	
	Fail[0] = -1;
	Fail[1] = 0;
	
	while( pos < pattern_size )
	{
		if( pattern[pos - 1] == pattern[cnd] )
			Fail[pos++] = ++cnd;
		else if( cnd > 0 )
			cnd = Fail[cnd];
		else
			Fail[pos++] = 0;
	}
	
	while( ( m + i ) < text_size )
	{
		if( pattern[i] == text[m + i] )
		{
			if( i == ( pattern_size - 1 ) )
				return m;
			i++;
		}
		else
		{
			m += i - Fail[i];
			if( Fail[i] > -1 )
				i = Fail[i];
			else
				i = 0;
		}
	}
	
	return -1;
}

/* TABULKA FUNKCII */

/**
 *  \brief Prida zabudovanu funkciu do tabulky funkcii
 *  
 *  \param [in] name meno funkcie
 *  \param [in] size dlzka retazca meno
 *  \param [in] param_count pocet parametrov
 *  \param [in] unlimited funkcia moze mat neobmedzene mnozstvo parametrov
 *  \param [in] builtin_id identifikator funkcie
 *  \return navratova hodnota je E_OK inak E_INETRPRET_ERROR v pripade chyby
 *  
 *  \details prida funkciu do binarneho stromu
 */
E_ERROR_TYPE AddBuiltinFunction( char *name,
                                 unsigned int size,
                                 unsigned int param_count,
                                 bool unlimited,
                                 E_ERROR_TYPE (*builtin_id)( T_DVAR[], int, T_DVAR *)
                                )
{
    PRINTD("%s()\n", __func__ );
    FTableNode *ptr;
    ptr = malloc( sizeof( FTableNode ) );
    if ( ptr == NULL )
        return E_INTERPRET_ERROR;
        
        
    ptr->metadata.name = name;
    ptr->metadata.name_size = size;
    ptr->metadata.state = E_BUILTIN;
    ptr->metadata.param_count = param_count;
    ptr->metadata.frame_count = param_count;
    ptr->metadata.unlimited_param = unlimited;
    ptr->metadata.builtin_id = builtin_id;
    ptr->metadata.fix_list = NULL;
    ptr->metadata.tape = NULL;
    ptr->lptr = ptr->rptr = NULL;
    
    if ( FT.btreeroot == NULL )
    {
        FT.btreeroot = ptr;
    }
    else
    {
        FTableNode **ptr_help;
        ptr_help =  &( FT.btreeroot );
        
        while ( *ptr_help != NULL )
        {
            int n = sstrcmp( (*ptr_help)->metadata.name, name, (*ptr_help)->metadata.name_size, size );
            if ( n > 0 )
            {
                //printf("doprava \n");
                ptr_help = &( (*ptr_help)->rptr );
            }
            else if ( n < 0 )
            {
                //printf("dolava \n");
                ptr_help = &( (*ptr_help)->lptr );
            }
            else
            {
                //printf("aktualizacna semantika ?? \n");
                free( ptr );
                return E_OK;
            }
        }
        *ptr_help = ptr; 
    }
    return E_OK;    
}


/**
 *  \brief vyhlada funkciu v tabulke, ak neexistuje tak ju prida
 *  
 *  \param [in] name meno funkcie
 *  \param [in] size dlzka mena
 *  \param [in] ptr_out nastavi pointer na metadata 
 *  \return navrati E_INTERPRET_ERROR v pripade chyby inak vrati E_OK
 *  
 *  \details TODO
 */
E_ERROR_TYPE LookupFunction(char *name, unsigned int size, unsigned int line, FTableData **ptr_out) {
    PRINTD("%s()\n", __func__ );
    FTableNode **ptr = &(FT.btreeroot);
    
    while( *ptr != NULL )
    {
        int n = sstrcmp( (*ptr)->metadata.name, name, (*ptr)->metadata.name_size, size );
        if ( n > 0 )
        {
            //printf("doprava \n");
            ptr = &( (*ptr)->rptr );
        }
        else if (n < 0)
        {
            //printf("dolava \n");
            ptr = &( (*ptr)->lptr );
        }
        else
        {
            //printf("gotcha !!\n");
            *ptr_out = &( (**ptr).metadata );
            return E_OK;
        }
    }
    
    /* pridam neznamu funkciu */
    FT.unknown_count++;
    /* zaregistrujem dalsiu funkciu */
    FT.count++;
    
    *ptr = malloc(sizeof( FTableNode ));
    if (*ptr == NULL)
    {
        *ptr_out = NULL;
        return E_INTERPRET_ERROR;
    }
    (*ptr)->metadata.tape = malloc(sizeof(Instruction));
    if ( (*ptr)->metadata.tape == NULL )
    {
        *ptr_out = NULL;
        return E_INTERPRET_ERROR;
    }
    (*ptr)->metadata.tape->next = NULL;
    (*ptr)->metadata.tape->opcode = DUMMY;
    
    (*ptr)->metadata.name = name;
    (*ptr)->metadata.name_size = size;
    (*ptr)->metadata.state = E_UNKNOWN;
    (*ptr)->metadata.param_count = -1; // maximalny pocet parametrov
    (*ptr)->metadata.frame_count = 0;
    (*ptr)->metadata.unlimited_param = 0;
    (*ptr)->metadata.fix_list = NULL;
    (*ptr)->metadata.first_line = line;
    (*ptr)->lptr = (*ptr)->rptr = NULL;
    *ptr_out = &((**ptr).metadata);
    return E_OK;
}
/**
 *  \brief pri rekurzivnom mazani odstrani uzol zo zoznamu
 *  
 *  \param [in] ptr ukazovatel na uzol v binarnom strome
 *  \return void
 *  
 *  \details iba pri rekurzii, nevolat samostatne
 */
void DeteteFunctionItem(FTableNode *ptr)
{
    PRINTD("%s()\n", __func__ );
    if ( ptr == NULL ) return;
    
    DeteteFunctionItem(ptr->lptr);
    DeteteFunctionItem(ptr->rptr);
    
    /* overit ci je co uvolnovat */
    if (ptr->metadata.state == E_BUILTIN)
    {
        free(ptr);
        return;
    }
    
    /* skutocna funkcia */
    /* uvolnit zoznam fixov */
    if (ptr->metadata.fix_list)
    {
        InstructionList *instptr = ptr->metadata.fix_list;
        InstructionList *instptr2 = instptr;
        while(instptr != NULL)
        {
            instptr2 = instptr->next;
            free(instptr);
            instptr = instptr2;
        }
    }
    /* uvolnit pasku */
    Instruction *instptr = ptr->metadata.tape;
    Instruction *instptr2 = instptr;
    while(instptr != NULL)
    {
        instptr2 = instptr->next;
        free(instptr);
        instptr = instptr2;
    }
    /* uvolnit seba */
    free(ptr);
}

/**
 *  \brief uvolni celu tabulku funkcii
 *  
 *  \return void
 *  
 *  \details TODO
 */
void DeleteFT(void)
{
    PRINTD("%s()\n", __func__ );
    DeteteFunctionItem(FT.btreeroot);
    FT.btreeroot = NULL;
    /* uvolnit instrukcie hlavneho programu */
    Instruction *ptr = FT.tape;
    Instruction *ptr_help = ptr;
    
    while( ptr!= NULL )
    {
        ptr_help = ptr->next;
        free(ptr);
        ptr = ptr_help;
    }
    FT.tape = NULL;
    /* dokonane */
}


/* END TABULKA FUNKCII */

/* TABULKA SYMBOLOV */
/**
 *  \brief Inicializuje tabulku symbolov
 *  
 *  \param [in] tree ukazovatel na deskriptor tabulky
 *  \return void
 *  
 *  \details Details
 */
void BTinit( STable  *tree )
{
    PRINTD("%s()\n", __func__ );
    tree->btreeroot = NULL;
    tree->counter = 0;
    return;
}


/**
 *  \brief Brief
 *  
 *  \param [in] tree deskriptor tabulky symbolov
 *  \param [in] name Pmeno
 *  \param [in] name_size dlzka mena
 *  \param [out] ptr_out nasatavi pointer na metadata
 *  \return vrati E_OK, v pripade chyby E_INTERPRET_ERROR
 *  
 *  \details void
 */
E_ERROR_TYPE BTfind( STable *tree,
                     char *name,
                     int name_size,
                     STableData** ptr_out
                    )
{
    PRINTD("%s()\n", __func__ );
    int i;
    STableNode *Root = tree->btreeroot;

    while ( Root != NULL )
    {
        if ( ( i = sstrcmp( name, Root->metadata.name, name_size, Root->metadata.name_size ) ) > 0 )
        {
            Root = Root->rptr;
        }
        else if ( i < 0 )
        {
            Root = Root->lptr;
        }
        else // nasiel
        {
            *ptr_out = &(Root->metadata);    //ak som nasiel danu premennu
            return E_OK;
        }
    }
    //tu sa dostavam pri chybe
    *ptr_out = NULL;
    return E_UNDEF_VAR;
}

/**
 *  \brief najde v tabulke symbolov polozku, ak tam nie je tak ju prida
 *  
 *  \param [in] tree deskriptor binarneho stromu
 *  \param [in] name meno premennej
 *  \param [in] name_size dlzka mena
 *  \param [out] ptr_out nastavi pointer na metadata
 *  \return vrati E_OK, v pripade chyby E_INTERPRET_ERROR
 *  
 *  \details Details
 */
E_ERROR_TYPE BTlookup( STable *tree,
                       char *name,
                       int name_size,
                       STableData **ptr_out,
                       bool *added  
                      )
{
    PRINTD("%s()\n", __func__ );
    STableNode *help2 = tree->btreeroot;   //help2 kvoli zjednoduseniu pristupu do pamate
    if ( help2 == NULL )
    {
        if ( ( tree->btreeroot = malloc( sizeof( STableNode ) ) ) == NULL )
        {
            *ptr_out = NULL;
            return E_INTERPRET_ERROR;
        }

        help2 = tree->btreeroot; //jednoduchsi pristup
        help2->metadata.name = name;
        help2->metadata.name_size = name_size;
        help2->metadata.offset = tree->counter++;
        help2->metadata.assigned = false;
        help2->rptr = help2->lptr = NULL;
        *ptr_out = &(help2->metadata);
        *added = true;
        return E_OK;
    }

    STableNode *help1;    //zjednodusenie pristupu do pamate
    int i;

    while ( 1 )   //zefektivnenie kodu
    {
        if ( ( i = sstrcmp( name, help2->metadata.name, name_size, help2->metadata.name_size ) ) > 0 )
        {
            help1 = help2;
            if ( ( help2 = help2->rptr ) != NULL )
                continue;

            if ( ( help1->rptr = malloc( sizeof( STableNode ) ) ) == NULL )
            {
                *ptr_out = NULL;
                return E_INTERPRET_ERROR;
            }

            *added = true;
            help2 = help1->rptr;    //pouzil som help2 ak by som este nieco 
                                    //potreboval dorabat aby som zachoval
                                    //pointer na vyssiu uroven stromu
            help2->metadata.name = name;
            help2->metadata.name_size = name_size;
            help2->metadata.assigned = false;
            help2->metadata.offset = tree->counter++;
            help2->rptr = help2->lptr = NULL;

            *ptr_out = &( help2->metadata );
            return E_OK;
        }
        else if ( i < 0 )
        {
            help1 = help2;
            if ( ( help2 = help2->lptr ) != NULL )
                continue;

            if ( ( help1->lptr = malloc( sizeof( STableNode ) ) ) == NULL )
            {
                *ptr_out = NULL;
                return E_INTERPRET_ERROR;
            }

            *added = true;
            help2 = help1->lptr;   //pouzil som help2 ak by som este nieco potreboval dorabat aby som zachoval pointer na vyssiu uroven stromu
            help2->metadata.name = name;
            help2->metadata.name_size = name_size;
            help2->metadata.assigned = false;
            help2->metadata.offset = tree->counter++;
            help2->rptr = help2->lptr = NULL;

            *ptr_out = &(help2->metadata);
            return E_OK;
        }
        else
        {   
            help2->metadata.assigned = true;
            *added = false;
            *ptr_out = &(help2->metadata);
            return E_OK;
        }
    }
}
/**
 *  \brief rekurzivne odstrani polozky tabulky symbolov
 *  
 *  \param [in] Root ukazovatel na uzol stromu
 *  \return void
 *  
 *  \details Details
 */
void Delete( STableNode *Root )
{
    PRINTD("%s()\n", __func__ );
    if ( Root != NULL )
    {
        Delete( Root->lptr );
        Delete( Root->rptr );
        free( Root );
    }
}
/**
 *  \brief odstrani tabulku symbolov
 *  
 *  \param [in] tree deskriptor binarneho stromu
 *  \return void
 *  
 *  \details Details
 */
void DeleteBT( STable *tree )
{
    PRINTD("%s()\n", __func__ );
    Delete( tree->btreeroot );
    tree->btreeroot = NULL;
    tree->counter = 0;
}