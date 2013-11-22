#include <stdio.h>
#include <stdlib.h>


#include "types.h"
#include "generator.h"
#include "scanner.h"
#include "debug.h"

const int FLEXIBLE_ARRAY_MEMBER = 32;
FTable FT;
STable *STableLocal;
STable *STableGlobal;
STable *SwitchSTable;
PtrStack *ptrstack;
MapTable *Localmap;
MapTable *Globalmap;
MapTable *SwitchMap; // vyhybka
enum gen_state State;
Instruction *GlobalTape; 
Instruction *LocalTape;
Instruction *SwitchTape; // vyhybka
FTableData *actualfunction;
FTableData *callfunction;
STableData *assignvar;

/* DEBUG */
#define DEBUG

#ifdef DEBUG 
    #define PRINTD(...) \
        do { \
            fprintf ( stderr, __VA_ARGS__ ); \
        } \
        while(0)
#else
    #define PRINTD(...) \
        do { \
        ; \
        } \
        while(0)
#endif

/*  END DEBUG */

void print_DVAR(T_DVAR *ptr)
{
    switch(ptr->type)
    {
        case VAR_UNDEF:
            printf("VAR_UNDEF\n");
            break;
        case VAR_INT:
            printf("VAR_INT: %d\n", ptr->data._int);
            break;
        case VAR_BOOL:
            printf("VAR_BOOL: %d\n", ptr->data._bool);
            break;
        case VAR_DOUBLE:
            printf("VAR_DOUBLE: %f\n", ptr->data._double);
            break;
        case VAR_STRING:
            printf("VAR_STRING size [%d]: '", ptr->size);
            for(unsigned int i = 0; i < ptr->size; i++ )
            {
                putchar(ptr->data._string[i]);
            }
            printf("'\n");
            break;
        case VAR_CONSTSTRING:
            printf("VAR_CONSTSTRING size [%d]: '", ptr->size);
            for(unsigned int i = 0; i < ptr->size; i++ )
            {
                putchar(ptr->data._string[i]);
            }
            printf("'\n");
            break;
        case VAR_LOCAL:
            printf("VAR_LOCAL: %u\n", ptr->data.offset);
            break;
        default:
            printf("UNKNOWN VAR -->FIX ME\n");
            break;
    }
}

void PrintTape( Instruction *ptr )
{
    while( ptr != NULL )
    {
        printf("----------------\n");
        printf("Opcode: %s\n", OPCODE_NAME[ptr->opcode]);
        switch( ptr->opcode )
        {
            case START:
            case CREATE:
                printf("Size: %u\n", ptr->attr.size);
                break;
            case CALL:
                printf("Call ptr: %p\n", (void*)ptr->attr.jump.jmp);
                 break;
            case CALL_BUILTIN:
                printf("Call index: %u\n", ptr->attr.builtin);
                 break;
            case JMP:
                printf("Jump ptr: %p\n", (void*)ptr->attr.jump.jmp);
                 break;
            default:
                printf("Dest: [%u]\n", ptr->attr.tac.dest);
                printf("OP1: ");
                print_DVAR( &( ptr->attr.tac.op1 ) );
                printf("OP2: ");
                print_DVAR( &( ptr->attr.tac.op2 ) );
        }
    ptr = ptr->next;
    }
    printf("** TAPE END **\n");
}

E_ERROR_TYPE GeneratorInit()
{
    /* Inicializacia zasobnikov a pomocnych struktur */
    if ( PtrStackInit( &ptrstack ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( MapTableInit( &Localmap ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( MapTableInit( &Globalmap ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    
    /* Vytvorenie tabuliek symbolov */
    if( (STableGlobal = malloc(sizeof(STable))) == NULL )
        return E_INTERPRET_ERROR;
    BTinit(STableGlobal);
    
    if( (STableLocal = malloc(sizeof(STable))) == NULL )
        return E_INTERPRET_ERROR;
    BTinit(STableLocal);
    
    /* hlavna instrukcna paska*/
    FT.tape = malloc(sizeof(Instruction));
    if (FT.tape == NULL)
    {
        return E_INTERPRET_ERROR;
    }
    
    
    FT.tape->next = NULL;
    FT.tape->opcode = START;
    FT.tape->attr.size = 0;
    
    /* nastavenia vyhybky */
    GlobalTape = FT.tape;
    SwitchTape = GlobalTape;
    SwitchMap = Globalmap; // najprv ukazuje na globalny priestor
    SwitchSTable = STableGlobal;
    State = S_DEFAULT;
    
    return E_OK;
}

void GeneratorErrorCleanup()
{
    if(ptrstack)
    {
        free(ptrstack);
        ptrstack = NULL;
    }
    if(Localmap)
    {
        free(Localmap);
        Localmap = NULL;
    }
    if(Globalmap)
    {
        free(Globalmap);
        Globalmap = NULL;
    }
    SwitchMap = NULL;
    if(STableGlobal)
    {
        DeleteBT(STableGlobal);
        free(STableGlobal);
        STableGlobal = NULL;
    }
    if(STableLocal)
    {
        DeleteBT(STableLocal);
        free(STableLocal);
        STableLocal = NULL;
    }
    SwitchSTable = NULL;
    DeleteFT();
}

double toDouble( T_token *token )
{
    switch(token->ttype)
    {
        case E_INT:
            return (double)token->data._int;
        case E_DOUBLE:
            return token->data._double;
        case E_NULL:
        case E_FALSE:
            return 0.0;
        case E_TRUE:
            return 1.0;
        case E_LITER:
            /*return doubleval(token);*/
        default:
            return 0.0;
    }
    return 0.0;
    
}

void translate_token( T_token *token, T_DVAR *out )
{
    switch( token->ttype )
    {
        case E_LOCAL:
            out->type = VAR_LOCAL;
            out->data.offset = token->length;
            break;
        case E_INT:
            out->type = VAR_INT;
            out->data._int = token->data._int;
            break;
        case E_DOUBLE:
            out->type = VAR_DOUBLE;
            out->data._int = token->data._double;
            break;
        case E_LITER:
            out->type = VAR_CONSTSTRING;
            out->data._string = token->data._string;
            out->size = token->length;
            break;
        case E_FALSE:
            out->type = VAR_BOOL;
            out->data._bool = false;
            break;
        case E_TRUE:
            out->type = VAR_BOOL;
            out->data._bool = true;
            break;
        case E_NULL:
            out->type = VAR_NULL;
            out->data._bool = true;
            break;
        default:
            PRINTD("ERROR translate_token() bad token type\n");
            break;
    }
}

E_ERROR_TYPE perform_eval_term(T_token *op)
{
    /* TODO */
    PRINTD("perform_eval_term()\n");
    
    // zistim ci sa da optimalizovat
    if ( op->ttype == E_LOCAL && 
         SwitchTape->opcode >= CONCAT && SwitchTape->opcode <= GREATEREQ && 
         SwitchTape->attr.tac.dest == op->length)
    {
        PRINTD("predosla instrukcia bola 3adresna, typ %s\n", OPCODE_NAME[SwitchTape->opcode]);
        SwitchTape->attr.tac.dest = assignvar->offset;
        free(op);
        return E_OK;
    }
    PRINTD("Generujem instrukciu mov\n");
    if( ( SwitchTape->next = malloc( sizeof(Instruction) ) ) == NULL )
    {
        return E_INTERPRET_ERROR;
    }
    SwitchTape = SwitchTape->next;
    
    SwitchTape->next = NULL;
    SwitchTape->opcode = MOV;
    SwitchTape->attr.tac.dest = assignvar->offset;
    SwitchTape->attr.tac.op2.type = VAR_NO_VAR; 
    
    switch( op->ttype )
    {
        case E_VAR:
        {
            E_ERROR_TYPE retval;
            STableData *op_ptr;
            if( (retval = BTfind( SwitchSTable, op->data._string, op->length, &op_ptr ) ) != E_OK )
            {
                free(op);
                return retval;
            }                
            SwitchTape->attr.tac.op1.type = VAR_LOCAL;
            SwitchTape->attr.tac.op1.data.offset = op_ptr->offset; // offset je v _int
            break;
        }
        case E_INT:
            SwitchTape->attr.tac.op1.type = VAR_INT; 
            SwitchTape->attr.tac.op1.data._int = op->data._int;
            break;
        case E_DOUBLE:
            SwitchTape->attr.tac.op1.type = VAR_DOUBLE; 
            SwitchTape->attr.tac.op1.data._double = op->data._double;
            break;
        case E_LITER:
            SwitchTape->attr.tac.op1.type = VAR_CONSTSTRING; 
            SwitchTape->attr.tac.op1.data._string = op->data._string;
            break;
        case E_LOCAL:
            SwitchTape->attr.tac.op1.type = VAR_LOCAL; 
            SwitchTape->attr.tac.op1.data.offset = op->data._int;
            break;
        case E_TRUE:
            SwitchTape->attr.tac.op1.type = VAR_BOOL; 
            SwitchTape->attr.tac.op1.data._bool = true; 
        case E_FALSE:
            SwitchTape->attr.tac.op1.type = VAR_BOOL; 
            SwitchTape->attr.tac.op1.data._bool = false; 
        default :
            PRINTD("eval() --> zly parameter pre TERM operaciu ");
            free(op);
            return E_OTHER;
            break;
    };
    free(op);
    SwitchTape->attr.tac.op2.type = VAR_NO_VAR;
    return E_OK;
}


E_ERROR_TYPE eval(T_token *op1, T_token *op2, TOKEN_TYPE operation)
{
    static unsigned int actual_usage = 0; // pocet pouzitych pomocnych premennych v jednom vyraze
    
    if ( operation == E_TERM )
    {
        actual_usage = 0;
        return perform_eval_term(op1);
    }
    
    if ( ( operation == E_CONCAT ) || 
         ( ( op1->ttype == E_VAR || op1->ttype == E_LOCAL ) &&
         ( op2->ttype == E_VAR || op2->ttype == E_LOCAL ) ) )
    {
        /* overit platnost premennych */
        STableData *op_ptr1 = NULL;
        STableData *op_ptr2 = NULL;
        if( op1->ttype == E_VAR )
        {
            int retval = BTfind(SwitchSTable, op1->data._string, op1->length, &op_ptr1);
            if( retval != E_OK )
            {
                free(op2);
                return retval;
            }
        }
        if( op2->ttype == E_VAR )
        {
            int retval = BTfind(SwitchSTable, op2->data._string, op2->length, &op_ptr2);
            if( retval != E_OK )
            {
                free(op2);
                return retval;
            }
        }
        
        // nova instrukcia
        Instruction *ptr;
        if ( ( ptr = malloc( sizeof( Instruction ) ) ) == NULL )
        {
            free(op2);
            return E_INTERPRET_ERROR;
        }
        
        ptr->next = NULL;
        SwitchTape->next = ptr;
        SwitchTape = ptr;
        
        unsigned int dest;
        /* zvolenie destination - nova destination iba ked su oba operandy VAR */
        if ( op_ptr1 != NULL || op_ptr2 != NULL )
        {
            if ( actual_usage >= SwitchMap->used_space ) // nova premenna
            {
                if ( MapTableCheck( &SwitchMap ) != E_OK )
                {
                    free(op2);
                    return E_INTERPRET_ERROR;
                }
                    
                PRINTD("actual_usage = %d, counter %d, maptable %d/%d\n", 
                        actual_usage, SwitchSTable->counter,
                        SwitchMap->used_space, SwitchMap->size);   
                
                dest = SwitchSTable->counter++;
                SwitchMap->map[actual_usage++] = SwitchSTable->counter;
                /* namapovana nova expr. premenna */
                SwitchMap->used_space++;
                PRINTD("actual_usage = %d, counter %d, maptable %d/%d\n", 
                        actual_usage, SwitchSTable->counter,
                        SwitchMap->used_space, SwitchMap->size);
            }
            else // vyuzije sa existujuca premenna
            {
                dest = SwitchMap->map[actual_usage++];
            }  
        }
        else
        {
            if ( op1->ttype == E_LOCAL )
                dest = op1->data._int;
            else
                dest = op2->data._int;
        }
        SwitchTape->attr.tac.dest = dest;
        /* operand 1 */
        if ( op_ptr1 == NULL )
        {
            translate_token(op1, &(SwitchTape->attr.tac.op1) );
        }
        else
        {
            SwitchTape->attr.tac.op1.type = VAR_LOCAL;
            SwitchTape->attr.tac.op1.data.offset = op_ptr1->offset;
        }
        /* operand 2 */
        if ( op_ptr2 == NULL )
        {
            translate_token(op2, &(SwitchTape->attr.tac.op2) );
        }
        else
        {
            SwitchTape->attr.tac.op2.type = VAR_LOCAL;
            SwitchTape->attr.tac.op2.data.offset = op_ptr2->offset;
        }
        
        op1->ttype = E_LOCAL;
        op1->length = dest;
        
        PRINTD("OPERATION = %s\n", TOKEN_NAME[operation]);
        switch(operation)
        {
            case E_CONCAT:
                SwitchTape->opcode = CONCAT;
                break;
            case E_TRIPLEEQ:
                SwitchTape->opcode = EQUAL;
                break;
            case E_NOT_EQ:
                SwitchTape->opcode = NONEQUAL;
                break;
            case E_PLUS:
                SwitchTape->opcode = PLUS;
                break;
            case E_MULT:
                SwitchTape->opcode = MUL;
                break;
            case E_MINUS:
                SwitchTape->opcode = MINUS;
                break;
            case E_DIV:
                SwitchTape->opcode = DIV;
                break;
            case E_LESS:
                SwitchTape->opcode = LESS;
                break;
            case E_GREATER:
                SwitchTape->opcode = GREATER;
                break;
            case E_LESSEQ:
                SwitchTape->opcode = LESSEQ;
                break;
            case E_GREATEREQ:
                SwitchTape->opcode = GREATEREQ;
                break;
            default:
                PRINTD("eval() --> invalid operation %s\n", TOKEN_NAME[operation]);
                break;
        }
        free(op2);
        return E_OK;
    }
    
    switch(operation)
    {
        case E_PLUS:
            {
                if ( op1->ttype == E_DOUBLE || op2->ttype == E_DOUBLE )
                {
                    op1->data._double = toDouble(op1) + toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int += op2->data._int;
                }
                else
                {
                   free(op2);
                   return E_INCOMPATIBLE; 
                }
                free(op2);
                break;   
            }
        case E_MINUS:
            {
                if ( op1->ttype == E_DOUBLE || op2->ttype == E_DOUBLE )
                {
                    op1->data._double = toDouble(op1) - toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int -= op2->data._int;
                }
                else
                {
                   free(op2);
                   return E_INCOMPATIBLE; 
                }
                free(op2);
                break;   
            }
        case E_MULT:
            {
                if ( op1->ttype == E_DOUBLE || op2->ttype == E_DOUBLE )
                {
                    op1->data._double = toDouble(op1) * toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int *= op2->data._int;
                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;   
            }
        case E_DIV:
            {
                if ( (op1->ttype == E_DOUBLE || op1->ttype == E_INT ) &&
                     (op2->ttype == E_DOUBLE || op2->ttype == E_INT )  )
                {
                    op1->data._double = toDouble(op1) / toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;
            }
        case E_LESS:
            {
                if ( op1->ttype == op2->ttype )
                {
                    bool val = false;
                    
                    switch( op1->ttype )
                    {
                        case E_INT:
                            val = ( op1->data._int ) < ( op2->data._int );
                            break; //handbrake;
                        case E_DOUBLE:
                            val = (op1->data._double) < ( op2->data._double );
                            break;
                        case E_LITER:
                            val = true; // TODO
                            break;
                        default:
                            break;
                    }
                    if( val )
                        op1->ttype = E_TRUE;
                    else
                        op1->ttype = E_FALSE;
                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;
                
            }
        case E_GREATER:
            {
                if ( op1->ttype == op2->ttype )
                {
                    bool val = false;
                    
                    switch( op1->ttype )
                    {
                        case E_INT:
                            val = ( op1->data._int ) > ( op2->data._int );
                            break; //handbrake;
                        case E_DOUBLE:
                            val = (op1->data._double) > ( op2->data._double );
                            break;
                        case E_LITER:
                            val = true; // TODO
                            break;
                        default:
                            break;
                    }
                    if( val )
                        op1->ttype = E_TRUE;
                    else
                        op1->ttype = E_FALSE;
                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;
                
            }
        case E_LESSEQ:
            {
                if ( op1->ttype == op2->ttype )

                {
                    bool val = true;
                    
                    switch( op1->ttype )
                    {
                        case E_INT:
                            val = ( op1->data._int ) <= ( op2->data._int );
                            break; //handbrake;
                        case E_DOUBLE:
                            val = (op1->data._double) <= ( op2->data._double );
                            break;
                        case E_LITER:
                            val = true; // TODO
                            break;
                        default:
                            break;
                    }
                    if( val )
                        op1->ttype = E_TRUE;
                    else
                        op1->ttype = E_FALSE;


                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;
                
            }
        case E_GREATEREQ:
            {
                if ( op1->ttype == op2->ttype )
                {
                    bool val = true;
                    
                    switch( op1->ttype )
                    {
                        case E_INT:
                            val = ( op1->data._int ) >= ( op2->data._int );
                            break; //handbrake;
                        case E_DOUBLE:
                            val = (op1->data._double) >= ( op2->data._double );
                            break;
                        case E_LITER:
                            val = true; // TODO
                            break;
                        default:
                            break;
                    }
                    if( val )
                        op1->ttype = E_TRUE;
                    else
                        op1->ttype = E_FALSE;
                }
                else
                {
                    free(op2);
                    return E_INCOMPATIBLE; 
                }
                free(op2);
                break;
                
            }
        case E_TRIPLEEQ:
        case E_NOT_EQ:
            {
                bool val = false;
                if ( op1->ttype == op2->ttype )
                {
                    val = true;
                    switch(op1->ttype)
                    {
                        case E_INT:
                            if (op1->data._int != op2->data._int)
                                val = false;
                            break;
                        case E_DOUBLE:
                            if ( op1->data._double != op2->data._double)
                                val = false;
                            break;
                        case E_LITER:
                            if ( sstrcmp( op1->data._string, op2->data._string, op1->length, op2->length ) != 0 )
                                val = false;
                            break;
                        default:
                            break;
                    }
                    
                }
                else
                if (operation == E_NOT_EQ)
                    val = !val;
                if( val )
                    op1->ttype = E_TRUE;
                else
                    op1->ttype = E_FALSE;
                free(op2);
                break;
            }            

        default:
            fprintf(stderr, "Invalid operation in EVAL(), operation = %d \n", operation);
    }
    return E_OK;
}

E_ERROR_TYPE assing(T_token *op1)
/* nastavuje globalnu premennu */
{
    PRINTD("assigning, name %.3s\n", op1->data._string);
    if ( BTlookup(SwitchSTable , op1->data._string, op1->length, &assignvar ) != E_OK )
        return E_INTERPRET_ERROR;
    PRINTD("name %.3s got id %d\n", op1->data._string, assignvar->offset);
        
    return E_OK;
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
                                 enum builtin_functions builtin_id
                                )
{
    FTableNode *ptr;
    ptr = malloc( sizeof( FTableNode ) );
    if ( ptr == NULL )
        return E_INTERPRET_ERROR;
        
        
    ptr->metadata.name = name;
    ptr->metadata.name_size = size;
    ptr->metadata.state = E_BUILTIN;
    ptr->metadata.param_count = param_count;
    ptr->metadata.unlimited_param = unlimited;
    ptr->metadata.builtin_id = builtin_id;
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
E_ERROR_TYPE LookupFunction(char *name, unsigned int size,  FTableData **ptr_out) {

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
    
    *ptr = malloc(sizeof( FTableNode ));
    if (*ptr == NULL)
    {
        *ptr_out = NULL;
        return E_INTERPRET_ERROR;
    }
    (*ptr)->metadata.tape = malloc(sizeof(Instruction));
    // todo DUMMY instruction
    if ( (*ptr)->metadata.tape == NULL )
    {
        *ptr_out = NULL;
        return E_INTERPRET_ERROR;
    }
    (*ptr)->metadata.tape->next = NULL;
    
    (*ptr)->metadata.name = name;
    (*ptr)->metadata.name_size = size;
    (*ptr)->metadata.state = E_UNKNOWN;
    (*ptr)->metadata.param_count = 0;
    (*ptr)->metadata.param_count = 0;
    (*ptr)->metadata.fix_list = NULL;
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
    DeteteFunctionItem(FT.btreeroot);
    /* uvolnit instrukcie hlavneho programu */
    Instruction *ptr = FT.tape;
    Instruction *ptr_help = ptr;
    
    while( ptr!= NULL )
    {
        ptr_help = ptr->next;
        free(ptr);
        ptr = ptr_help;
    }
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
                       STableData **ptr_out  
                      )
{
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
        help2->rptr = help2->lptr = NULL;
        *ptr_out = &(help2->metadata);
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

            help2 = help1->rptr;    //pouzil som help2 ak by som este nieco 
                                    //potreboval dorabat aby som zachoval
                                    //pointer na vyssiu uroven stromu
            help2->metadata.name = name;
            help2->metadata.name_size = name_size;
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

            help2 = help1->lptr;   //pouzil som help2 ak by som este nieco potreboval dorabat aby som zachoval pointer na vyssiu uroven stromu
            help2->metadata.name = name;
            help2->metadata.name_size = name_size;
            help2->metadata.offset = tree->counter++;
            help2->rptr = help2->lptr = NULL;

            *ptr_out = &(help2->metadata);
            return E_OK;
        }
        else
        {
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
    Delete( tree->btreeroot );
    tree->btreeroot = NULL;
    tree->counter = 0;
}

/**
 *  \brief inicializuje zasobnik pointrov
 *  
 *  \param [in/out] ptr nastavi pointer na zasobnik
 *  \return vrati E_OK, v pripade chyby E_INTERPRET_ERROR
 *  
 *  \details Details
 */
E_ERROR_TYPE PtrStackInit(PtrStack **ptr)
{
    *ptr = malloc( sizeof(PtrStack)+
            FLEXIBLE_ARRAY_MEMBER * sizeof(Instruction*) );
    if (*ptr == NULL)
        return E_INTERPRET_ERROR;
        
    (*ptr)->size = FLEXIBLE_ARRAY_MEMBER;
    (*ptr)->top = 0;
    return E_OK;
}

/**
 *  \brief Skontroluje, ci je v zasobniku este miesto
 *  
 *  \param [in/out] ukazovatel na zasobnik 
 *  \return E_OK, v priprade chyby vrati E_INTERPRET_ERROR
 *  
 *  \details realokuje povodne miesto a nastavi novy pointer na zasobnik
 */
E_ERROR_TYPE PtrStackCheck(PtrStack **ptr)
{
    if((*ptr)->size < (*ptr)->top)
    {
        PtrStack *tmp = *ptr;
        
        *ptr =  realloc (*ptr, sizeof( Instruction *) * ((*ptr)->size)*2
                         + sizeof( PtrStack ));
        if (*ptr == NULL) // ak realloc zlyha tak neuvolnuje pamat
        {
            free(tmp);
            return E_INTERPRET_ERROR;
        }
        (*ptr)->size = (*ptr)->size *2;
    }
    return E_OK;  
}

/**
 *  \brief Inicializuje mapovaciu tabulku
 *  
 *  \param [in/out] nastavi pointer na tabulku
 *  \return E_OK, v priprade chyby vrati E_INTERPRET_ERROR
 *  
 *  \details mallocuje novu tabulku
 */
E_ERROR_TYPE MapTableInit(MapTable **ptr)
{
    *ptr = malloc(sizeof( MapTable )+
            FLEXIBLE_ARRAY_MEMBER*sizeof(int));
    if (*ptr == NULL)
        return E_INTERPRET_ERROR;
        
    (*ptr)->size = FLEXIBLE_ARRAY_MEMBER;
    (*ptr)->used_space = 0;
    return E_OK;
}
/**
 *  \brief skontroluje ci netreba zvacsit mapovaciu tabulku
 *  
 *  \param [in/out] nastavi pointer na novu tabulku
 *  \return E_OK, v priprade chyby vrati E_INTERPRET_ERROR
 *  
 *  \details reallocuje tabulku na dvojnasobnu velkost
 */
E_ERROR_TYPE MapTableCheck(MapTable **ptr)
{
    if((*ptr)->size < (*ptr)->used_space)
    {
        MapTable *tmp = *ptr;
        
        *ptr =  realloc (*ptr, sizeof(int)*((*ptr)->size)*2 + sizeof(MapTable));
        if (*ptr == NULL) // ak realloc zlyha tak neuvolnuje pamat
        {
            free(tmp);
            return E_INTERPRET_ERROR;
        }
        (*ptr)->size = (*ptr)->size *2;
    }
    return E_OK;  
}






