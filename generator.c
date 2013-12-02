#include <stdio.h>
#include <stdlib.h>


#include "types.h"
#include "generator.h"
#include "scanner.h"
#include "built-in.h"

const int FLEXIBLE_ARRAY_MEMBER = 1;
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

unsigned int actual_usage = 0; // pocet pouzitych pomocnych premennych v jednom vyraze

/* DEBUG */
// #define DEBUG
#include "debug.h"
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
        case VAR_NULL:
            printf("VAR_NULL\n");
            break;
        default:
            printf("UNKNOWN VAR\n");
            break;
    }
}

void PrintTape( Instruction *ptr )
{
    while( ptr != NULL )
    {
        printf("-----------------------\n");
        printf("ADRESA %p\n", (void*)ptr);
        printf("Opcode: %s\n", OPCODE_NAME[ptr->opcode]);
        switch( ptr->opcode )
        {
            case START:
            case CREATE:
                printf("Size: %u\n", ptr->attr.size);
                break;
            case CALL:
                printf("Call ptr: %p\n", (void*)ptr->attr.jump.jmp);
                printf("Dest: [%u]\n", ptr->attr.jump.dest);
                 break;
            case CALL_BUILTIN:
            {
                union {
                    E_ERROR_TYPE (*fptr)( T_DVAR[], int, T_DVAR *);
                    void * ptr;
                } tmp;
                tmp.fptr = ptr->attr.builtin.func;
                printf("Call C ptr: %p\n", tmp.ptr);
                printf("Dest: [%u]\n", ptr->attr.builtin.dest);
                 break;
            }
            case JMP:
                printf("Jump ptr: %p\n", (void*)ptr->attr.jump.jmp);
                 break;
            case PUSH:
                printf("Dest: [%u]\n", ptr->attr.tac.dest);
                printf("OP1: ");
                print_DVAR( &( ptr->attr.tac.op1 ) );
                break;
            /*case MOVRET:
                printf("Dest: [%u]\n", ptr->attr.tac.dest);
                break;*/
            case COND:
                printf("Jump to: [%p]\n", (void*)ptr->attr.jump.jmp );
                printf("OP1: ");
                print_DVAR( &( ptr->attr.jump.op1 ) );
                break;
            case RET:
                printf("OP1: ");
                print_DVAR( &( ptr->attr.jump.op1 ) );
                break;
            case DUMMY:
                break;
            case MOV:
                printf("Dest: [%u]\n", ptr->attr.tac.dest);
                printf("OP1: ");
                print_DVAR( &( ptr->attr.tac.op1 ) );
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
    PRINTD("%s()\n", __func__ );
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
    
    if ( AddBuiltinFunction( "boolval", 7, 1, false, boolval ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "doubleval", 9, 1, false, doubleval ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "intval", 6, 1, false, intval ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "strval", 6, 1, false, strval ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "get_string", 10, 0, false, get_string ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "put_string", 10, 0, true, put_string ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "strlen", 6, 1, false, runtime_strlen ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "get_substring", 13, 3, false, get_substring ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "find_string", 11, 2, false, find_string ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    if ( AddBuiltinFunction( "sort_string", 11, 1, false, sort_string ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    return E_OK;
}

void GeneratorErrorCleanup()
{
    PRINTD("%s()\n", __func__ );
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

void fixtape(FTableNode *ptr, Instruction* array[])
{
    PRINTD("%s()\n", __func__ );
    static unsigned int index = 1;
    if ( ptr == NULL )
        return;
    if( !ptr->metadata.tape )
    {
        fixtape( ptr->lptr, array );
        fixtape( ptr->rptr, array );
        free( ptr );
        return;
    }
    array[index++] = ptr->metadata.tape;
    
    struct instruction_list *fix = ptr->metadata.fix_list;
    struct instruction_list *fix_free;
    while( fix != NULL )
    {
        Instruction *iptr = fix->instr;
        /* iptr mieri na instrukciu create */
        iptr->attr.size = ptr->metadata.frame_count;
        for( unsigned int i = 0; i < ptr->metadata.param_count; i++ )
            iptr = iptr->next;
        /* ak som na instrukcii push tak ich uvolnim a prepojim zoznam */
        Instruction *iptr_start = iptr;
        
        iptr = iptr->next;
        Instruction *iptr_free = iptr;
        
        while( iptr->opcode != CALL )
        {
            iptr_free = iptr;
            iptr = iptr->next;
            free( iptr_free );
        }
        iptr_start->next = iptr;
        fix_free = fix;
        fix = fix->next;
        free( fix_free );
    }
    
    /*  rekurzivne uvolnenie */
    fixtape( ptr->lptr, array );
    fixtape( ptr->rptr, array );
    free( ptr );

}

E_ERROR_TYPE GeneratorPrepareTape(struct InstructionTapeBuffer **ptr)
{
    PRINTD("%s()\n", __func__ );
    *ptr = malloc( sizeof(struct InstructionTapeBuffer) + sizeof(Instruction *) * (FT.count+1) );
    if ( *ptr == NULL )
    {
        return E_INTERPRET_ERROR;
    }
    (*ptr)->size = FT.count+1;
    (*ptr)->array[0] = FT.tape; // hlavna paska
    fixtape(FT.btreeroot,(*ptr)->array );
    FT.btreeroot = NULL;
    FT.tape = NULL;
    GeneratorErrorCleanup();
    return E_OK; 
}

void GeneratorDeleteTapes(struct InstructionTapeBuffer *ptr)
{
    PRINTD("%s()\n", __func__ );
    for( unsigned int i = 0; i < ptr->size; i++)
    {
        Instruction *iptr = ptr->array[i];
        Instruction *iptr_free = iptr;
        
        while( iptr != NULL)
        {
            iptr_free = iptr;
            iptr = iptr->next;
            free(iptr_free);
        }
    }
    free(ptr);
}

static inline double toDouble( T_token *token )
{
    PRINTD("%s()\n", __func__ );
    switch(token->ttype)
    {
        case E_INT:
            return (double)token->data._int;
        case E_DOUBLE:
            return token->data._double;
        default:
            return 0.0;
    }
    return 0.0;
}

void translate_token( T_token *token, T_DVAR *out )
{
    PRINTD("%s()\n", __func__ );
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
            out->data._double = token->data._double;
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
            ERROR( "ERROR translate_token() bad token type %s\n", TOKEN_NAME[token->ttype]);
            break;
    }
}

E_ERROR_TYPE AddInstruction(  )
{
    PRINTD("%s()\n", __func__ );
    if ( SwitchTape->opcode != DUMMY )
    {
        if( ( SwitchTape->next = malloc( sizeof(Instruction) ) ) == NULL )
        {
            return E_INTERPRET_ERROR;
        }
        SwitchTape = SwitchTape->next;
        SwitchTape->next = NULL;
    }
    return E_OK;
}

void SwitchContextToFunction( void )
{
    PRINTD("%s()\n", __func__ );
    SwitchSTable = STableLocal;
    SwitchMap = Localmap;
    GlobalTape = SwitchTape;
    SwitchTape = actualfunction->tape;
    State = S_DEFAULT;
    assignvar = NULL;
}
void SwitchContextToGobal( void )
{
    PRINTD("%s()\n", __func__ );
    DeleteBT( SwitchSTable );
    SwitchSTable = STableGlobal;
    SwitchMap->used_space = 0;
    SwitchMap = Globalmap;
    SwitchTape = GlobalTape;
    actualfunction = NULL;
    State = S_DEFAULT;
    assignvar = NULL;
}

E_ERROR_TYPE setstate(enum gen_state state)
{
    PRINTD("%s()\n", __func__ );
    switch( state )
    {
        case S_DEFAULT:
            state = S_DEFAULT;
            return E_OK;
            break;
        case S_IF_BEGIN: // spracuje sa az v eval terme
            State = state;
            return E_OK;
        case S_IF_ELSE: // vygenrovat skok a prepojit cond na dummy instrukciu
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = JMP;
            SwitchTape->attr.jump.jmp = NULL;
            SwitchTape->next = NULL;
            /* mam jump, treba pamatat jump*/
            if( PtrStackCheck(&ptrstack) != E_OK )
            {
                return E_INTERPRET_ERROR;
            }
            ptrstack->stack[ptrstack->top++] = SwitchTape;
            PRINTD("Pointer pridany na zasobnik\n");
            /* pridat dummy*/
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = DUMMY;
            SwitchTape->next = NULL;
            /* prelinkujem to co viem*/
            /* nastavim false vetvu*/
            ptrstack->stack[ ptrstack->top -2 ]->attr.jump.jmp = SwitchTape;
            State = S_DEFAULT;
            return E_OK;
        case S_IF_END: // koniec false vetvy -> treba vygerenovat dummy a napojit to na jmp z true vetvy
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = DUMMY;
            SwitchTape->next = NULL;
            /* napojim skok na dummy instrukciu*/
            ptrstack->stack[ ptrstack->top -1 ]->attr.jump.jmp = SwitchTape;
            ptrstack->top -= 2; //nepotrebujem posledne 2 pointre
            State = S_DEFAULT;
            return E_OK;
        case S_WHILE_BEGIN: // zapamatat prvu instrukciu - pridam dummy, nemenim stav
                // v evale vygenerujem cond
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = DUMMY;
            SwitchTape->next = NULL;
            /* musim si ju zapamatat */
            if( PtrStackCheck(&ptrstack) != E_OK )
            {
                return E_INTERPRET_ERROR;
            }
            ptrstack->stack[ptrstack->top++] = SwitchTape;
            PRINTD("Pointer pridany na zasobnik\n");
            State = S_WHILE_BEGIN;
            return E_OK;
        case S_WHILE_END: // prelinkujem while, pridam jmp
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            /* pridam jmp na konci while a napojim na zaciatok vyrazu */
            SwitchTape->opcode = JMP;
            SwitchTape->attr.jump.jmp = ptrstack->stack[ ptrstack->top -2 ];
            SwitchTape->next = NULL;
            /* pridam dummy a nastavim na nu cond jump */
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = DUMMY;
            SwitchTape->next = NULL;
            ptrstack->stack[ ptrstack->top -1 ]->attr.jump.jmp = SwitchTape;
            ptrstack->top -=2;
            State = S_DEFAULT;
            return E_OK;
        case S_FUNCTION_END:
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = RET;
            SwitchTape->attr.jump.op1.type = VAR_NULL;
            State = state; // pridat return
            actualfunction->frame_count = SwitchSTable->counter;
            SwitchContextToGobal();
            break;
        case S_FILE_END: // pridam default retval
            if ( AddInstruction() != E_OK )
                return E_INTERPRET_ERROR;
            SwitchTape->opcode = RET;
            SwitchTape->attr.jump.op1.type = VAR_NULL;
            /* nastavim velkost ramca prvej instrukcie */
            FT.tape->attr.size = SwitchSTable->counter;
            State = state;
            if ( FT.unknown_count >0 )
            {
                FindUnknownFunctions(FT.btreeroot);
                return E_SEM;
            }
            break;
    }
    return E_OK;
}

E_ERROR_TYPE define(T_token *token)
{
    PRINTD("%s()\n", __func__ );
    E_ERROR_TYPE retval;
    if( ( retval = LookupFunction( token->data._string, token->length, token->line, &actualfunction ) ) != E_OK)
    {   
        // chyba mallocu
        return E_INTERPRET_ERROR;
    }
    PRINTD( "new function declared %.1s\n", token->data._string );
    if ( actualfunction->state == E_UNKNOWN )
    {
        actualfunction->state = E_DEFINED;
        FT.unknown_count--;
    }
    else
    {
        ERROR("Error on line %u: Redefinition of function '", token->line );
        print_char( stderr, actualfunction->name, actualfunction->name_size );
        ERROR("'.\n");
        return E_SEM;
    }
    SwitchContextToFunction();
    return E_OK;
}

E_ERROR_TYPE addparam(T_token *token)
{
    PRINTD("%s()\n", __func__ );
    static unsigned int param_counter = 0;
    if ( token != NULL )
    {
        E_ERROR_TYPE retval;
        STableData *ptr;
        bool new_param;
        if ( ( retval = BTlookup( SwitchSTable, token->data._string, token->length, &ptr, &new_param ) ) != E_OK )
        {
            return retval;
        }
        
        if( !new_param )
        {
            ERROR("Error on line %u: Parameter with the name '$", token->line );
            print_char( stderr, token->data._string, token->length );
            ERROR("' already declared.\n");
            return E_OTHER;
        }
        ptr->assigned = true;
        param_counter++;
    }
    else
    {
        if ( param_counter <= actualfunction->param_count ) // (unsigned)-1 = max int
        {
            actualfunction->param_count = param_counter;
            param_counter = 0;
        }
        else
        {
            ERROR( "Error on line %u: function '", token->line );
            print_char( stderr, actualfunction->name, actualfunction->name_size );
            ERROR("' defined with %d parameters but called with %d parameters.\n", param_counter, actualfunction->param_count);
            return E_PARAM;
        }
    }
    return E_OK;
}

E_ERROR_TYPE perform_eval_term(T_token *op)
{
    PRINTD("%s()\n", __func__ );
    
    if ( State == S_DEFAULT && assignvar && op->ttype == E_LOCAL ) // zistim ci sa da optimalizovat
    {
        if ( SwitchTape->opcode == CALL && SwitchTape->attr.jump.dest == op->length )
        {
            PRINTD( "predosla instrukcia bola CALL\n" );
            SwitchTape->attr.jump.dest = assignvar->offset;
            assignvar->assigned = true;
            free(op);
            return E_OK;
        }
        else if ( SwitchTape->opcode == CALL_BUILTIN && SwitchTape->attr.builtin.dest == op->length )
        {
            PRINTD( "predosla instrukcia bola CALL_BUILTIN\n" );
            SwitchTape->attr.builtin.dest = assignvar->offset;
            assignvar->assigned = true;
            free(op);
            return E_OK;
        }
        else if ( SwitchTape->opcode >= CONCAT && SwitchTape->attr.tac.dest == op->length )
        {
            PRINTD("predosla instrukcia bola 3adresna, typ %s\n", OPCODE_NAME[SwitchTape->opcode]);
            SwitchTape->attr.tac.dest = assignvar->offset;
            assignvar->assigned = true;
            free(op);
            return E_OK;
        }
    }
    
    PRINTD("Generujem instrukciu termu\n");
    if ( AddInstruction( ) != E_OK )
    {
        free(op);
        return E_INTERPRET_ERROR;
    }
    
    T_DVAR *ptr;
    
    switch( State )
    {
        case S_DEFAULT:
            if( assignvar != NULL )
            {
                SwitchTape->opcode = MOV;
                SwitchTape->attr.tac.dest = assignvar->offset;
                //assignvar->assigned = true;
                SwitchTape->attr.tac.op2.type = VAR_NO_VAR;
                ptr = &( SwitchTape->attr.tac.op1 );
            }
            else
            {
                /* RETURN */
                SwitchTape->opcode = RET;
                SwitchTape->attr.jump.dest = 0;
                PRINTD("RETURN INSTRUCTION \n");
                 ptr = &( SwitchTape->attr.jump.op1 );
            }
            break;
        default:
            {
                SwitchTape->opcode = COND;
                SwitchTape->attr.jump.jmp = NULL;
                ptr = &( SwitchTape->attr.jump.op1 );
                if( PtrStackCheck(&ptrstack) != E_OK )
                {
                    return E_INTERPRET_ERROR;
                }
                ptrstack->stack[ptrstack->top++] = SwitchTape;
                PRINTD("Pointer pridany na zasobnik\n");
                State = S_DEFAULT; // nastavim normalny stav
                break;
            }
    } 
    
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
            if (op_ptr->assigned == false)
            {
                ERROR( "Error on line %u: Variable '$", op->line );
                print_char( stderr, op->data._string, op->length );
                ERROR("' is not defined.\n");
                free(op);
                return E_UNDEF_VAR;
            }
            ptr->type = VAR_LOCAL;
            ptr->data.offset = op_ptr->offset;
            break;
        } 
        default :
            translate_token( op, ptr );
            break;
    };
    if( assignvar != NULL )
    {
        assignvar->assigned = true;
        assignvar = NULL;
    }
    free(op);
    return E_OK;
}

E_ERROR_TYPE get_local_var(unsigned int *dest)
{
    PRINTD("%s(actual usage = %d)\n", __func__ , actual_usage);
    PRINTD("-------------------\n");
    //for (unsigned int i = 0; i < SwitchMap->used_space; i++)
        //PRINTD("[%d/%d]:%d\n", i, SwitchMap->size, SwitchMap->map[i] );
    PRINTD("-------------------\n");
    if ( actual_usage >= SwitchMap->used_space ) // nova premenna
    {
        if ( MapTableCheck( &SwitchMap ) != E_OK )
        {
            return E_INTERPRET_ERROR;
        }   
        PRINTD("actual_usage = %d, counter %d, maptable %d/%d\n", actual_usage, SwitchSTable->counter,
                SwitchMap->used_space, SwitchMap->size);   
        *dest = SwitchSTable->counter++;
        SwitchMap->map[actual_usage++] = *dest;
        /* namapovana nova expr. premenna */
        SwitchMap->used_space++;
        PRINTD("actual_usage = %d, counter %d, maptable %d/%d\n", 
                actual_usage, SwitchSTable->counter,
                SwitchMap->used_space, SwitchMap->size);
    }
    else // vyuzije sa existujuca premenna
    {
        *dest = SwitchMap->map[actual_usage++];
    }
    return E_OK;
}

E_ERROR_TYPE evalf(T_token *array[], unsigned int size)
// array[0] - funkcia
// array[1..(size)] - parametre
{
    PRINTD("%s()\n", __func__ );
    FTableData *func;
    E_ERROR_TYPE retval;
    if( ( retval = LookupFunction( array[0]->data._string, array[0]->length, array[0]->line, &func ) ) != E_OK)
    { // chyba mallocu
        for( unsigned int i = 1; i<= size; i++)
            free(array[i]);
        return E_INTERPRET_ERROR;
    }
    /* instrukcia create*/
    if ( AddInstruction( ) != E_OK )
    {
        for( unsigned int i = 1; i<= size; i++ )
            free(array[i]);
        return E_INTERPRET_ERROR;
    }
    SwitchTape->opcode = CREATE;
    PRINTD( "CREATE ADDED\n" );
    
    if ( func->state == E_UNKNOWN )
    {
        PRINTD("Calling unknown function %.1s \n", func->name );
        if ( func->param_count > (size) )
        {
            func->param_count = size;
        }
        /* nastavim fixlist na prvu instrukciu volania funkcie*/
        InstructionList *tmp = malloc( sizeof ( InstructionList ));
        if ( tmp == NULL )
        {
            for( unsigned int i = 1; i <= size; i++)
                free(array[i]);
            return E_INTERPRET_ERROR;
        }
        tmp->next = func->fix_list;
        tmp->instr = SwitchTape;
        func->fix_list = tmp;
    }
    else if( actualfunction == func )
    {
        /* nastavim fixlist na prvu instrukciu volania funkcie*/
        InstructionList *tmp = malloc( sizeof ( InstructionList ));
        if ( tmp == NULL )
        {
            for( unsigned int i = 1; i <= size; i++)
                free(array[i]);
            return E_INTERPRET_ERROR;
        }
        tmp->next = func->fix_list;
        tmp->instr = SwitchTape;
        func->fix_list = tmp;
    }
    
    /* pocet parametrov */
    if ( ( func->state == E_DEFINED || func->state == E_BUILTIN ) &&
         ( func->param_count > (size) ) && func->unlimited_param == 0 )
    {
        ERROR("Error on line %u: Function '", array[0]->line);
        print_char(stderr, func->name, func->name_size);
        ERROR( "' called with %d parameters, expecting %d.\n", (size), func->param_count );
        for( unsigned int i = 1; i <= size; i++)
            free(array[i]);
        return E_PARAM;
    }
    
    unsigned int params = 0;
    if( func->unlimited_param || func->state == E_UNKNOWN )
    {
        params = (size);
        SwitchTape->attr.size = size;
    }
    else
    {
        params = func->param_count;
        SwitchTape->attr.size = func->frame_count;
    }
    
    for( unsigned int i = 1; i <= params; i++)
    {
        if ( AddInstruction( ) != E_OK )
        {
            for( unsigned int i = 1; i<= size; i++ )
                free(array[i]);
            return E_INTERPRET_ERROR;
        }
        SwitchTape->opcode = PUSH;
        SwitchTape->attr.tac.dest = i-1;
        if ( array[i]->ttype == E_VAR )
        {
            STableData *var;
            if( ( retval = BTfind(SwitchSTable, array[i]->data._string, array[i]->length, &var) ) != E_OK )
            {
                if ( retval == E_UNDEF_VAR )
                {
                     ERROR( "Error on line %u: Variable '$", array[i]->line );
                    print_char( stderr, array[i]->data._string, array[i]->length );
                    ERROR("' is not defined.\n");
                }
                for( unsigned int i = 1; i <= size; i++ )
                    free(array[i]);
                return retval;
            }
            if (var->assigned == false)
            {
                ERROR( "Error on line %u: Variable '$", array[i]->line );
                print_char( stderr, array[i]->data._string, array[i]->length );
                ERROR("' is not defined.\n");
                for( unsigned int i = 1; i <= size; i++ )
                    free(array[i]);
                return E_UNDEF_VAR;
            }
            SwitchTape->attr.tac.op1.type = VAR_LOCAL;
            SwitchTape->attr.tac.op1.data.offset = var->offset;
        }
        else
        {  
            translate_token( array[i], &( SwitchTape->attr.tac.op1 ) );
        }
    }
    
    /* otestovanie dalsich parametrov, aj ked sa nebudu pouzivat*/
    for( unsigned int i = params+1; i <= size; i++)
    {
        if ( array[i]->ttype == E_VAR )
        {
            STableData *var;
            if( ( retval = BTfind(SwitchSTable, array[i]->data._string, array[i]->length, &var) ) != E_OK )
            {
                for( unsigned int i = 1; i <= size; i++ )
                    free(array[i]);
                return retval;
            }
            if (var->assigned == false)
            {
                ERROR( "Error on line %u: Variable '$", array[i]->line );
                print_char( stderr, array[i]->data._string, array[i]->length );
                ERROR("' is not defined.\n");
                for( unsigned int i = 1; i <= size; i++ )
                    free(array[i]);
                return E_UNDEF_VAR;
            }
        }
    }
    
    /* instrukcia call*/
    if ( AddInstruction( ) != E_OK )
    {
        for( unsigned int i = 1; i<= size; i++ )
            free(array[i]);
        return E_INTERPRET_ERROR;
    }
    unsigned int *dest;
    if( func->state == E_BUILTIN )
    {
        SwitchTape->opcode = CALL_BUILTIN;
        SwitchTape->attr.builtin.func = func->builtin_id;
        dest = &( SwitchTape->attr.builtin.dest );
        
    }
    else
    {
        SwitchTape->opcode = CALL;
        SwitchTape->attr.jump.jmp = func->tape;
        dest = &( SwitchTape->attr.jump.dest );
    }
    
    /* uvolnim polozky*/
    for( unsigned int i = 1; i<= size; i++ )
            free(array[i]);
            
    /* retval instrukcia*/
    /*
    if ( AddInstruction( ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }*/
    
    if( ( retval = get_local_var( dest ) ) != E_OK)
    {
        return retval;
    }
    array[0]->ttype = E_LOCAL;
    array[0]->length = *dest;
    
    return E_OK;
}

E_ERROR_TYPE eval(T_token *op1, T_token *op2, TOKEN_TYPE operation)
{
    PRINTD( "%s( operation %s )\n", __func__ , TOKEN_NAME[operation] );
    if ( operation == E_TERM )
    {
        actual_usage = 0;
        return perform_eval_term(op1);
    }
    if ( ( operation == E_CONCAT ) || 
         ( ( op1->ttype == E_VAR || op1->ttype == E_LOCAL ) ||
         ( op2->ttype == E_VAR || op2->ttype == E_LOCAL ) ) )
    {
        PRINTD( "%s---- op1 %s op2 %s -----\n", __func__, TOKEN_NAME[op1->ttype], TOKEN_NAME[op2->ttype] );
        /* overit platnost premennych */
        STableData *op_ptr1 = NULL;
        STableData *op_ptr2 = NULL;
        if( op1->ttype == E_VAR )
        {
            int retval = BTfind(SwitchSTable, op1->data._string, op1->length, &op_ptr1);
            if( retval != E_OK || op_ptr1->assigned == false )
            {
                ERROR( "Error on line %u: Undefined variable $", op1->line );
                print_char( stderr, op1->data._string, op1->length );
                ERROR( " used in expression.\n" );
                free(op2);
                return E_UNDEF_VAR;
            }
            
        }
        if( op2->ttype == E_VAR )
        {
            int retval = BTfind(SwitchSTable, op2->data._string, op2->length, &op_ptr2);
            if( retval != E_OK || op_ptr2->assigned == false )
            {
                ERROR( "Error on line %u: Undeclared variable '$", op2->line );
                print_char( stderr, op2->data._string, op2->length );
                ERROR( "' used in expression.\n" );
                free(op2);
                return E_UNDEF_VAR;
            }
        }
        
        // nova instrukcia
        if ( AddInstruction( ) != E_OK )
        {
            free(op2);
            return E_INTERPRET_ERROR;
        }
        
        unsigned int dest;
        /* zvolenie destination - nova destination ked nie je ziaden operand E_LOCAL */
        if ( op1->ttype != E_LOCAL && op2->ttype != E_LOCAL )
        {
            E_ERROR_TYPE retval;
            if( ( retval = get_local_var( &dest ) ) != E_OK )
            {
                free(op2);
            }
        }
        else
        {
            if ( op1->ttype == E_LOCAL )
                dest = op1->length;
            else
                dest = op2->length;
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
                /* konkatenovat mozem len ked je prvy string alebo lokalna premenna */
                if( SwitchTape->attr.tac.op1.type ==  VAR_LOCAL ||
                    SwitchTape->attr.tac.op1.type ==  VAR_CONSTSTRING )
                {
                    SwitchTape->opcode = CONCAT;
                }
                else
                {
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
                    free(op2);
                    return E_SEM;
                }
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
                free(op2);
                return E_SEM;
                PRINTD("eval() --> invalid operation %s\n", TOKEN_NAME[operation]);
                break;
        }
        free(op2);
        return E_OK;
    }
    /* optimalizacia */
    
    switch(operation)
    {
        case E_PLUS:
            {
                if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int += op2->data._int;
                }
                else if( (op1->ttype == E_DOUBLE || op1->ttype == E_INT ) &&
                         (op2->ttype == E_DOUBLE || op2->ttype == E_INT )     )
                {
                    op1->data._double = toDouble(op1) + toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else
                {
                   ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
                   free(op2);
                   return E_INCOMPATIBLE; 
                }
                free(op2);
                break;   
            }
        case E_MINUS:
            {
                if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int -= op2->data._int;
                }
                else if( (op1->ttype == E_DOUBLE || op1->ttype == E_INT ) &&
                         (op2->ttype == E_DOUBLE || op2->ttype == E_INT )     )
                {
                    op1->data._double = toDouble(op1) - toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else
                {
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
                   free(op2);
                   return E_INCOMPATIBLE; 
                }
                free(op2);
                break;   
            }
        case E_MULT:
            {
                if ( op1->ttype == E_INT && op2->ttype == E_INT )
                {
                    op1->data._int *= op2->data._int;
                }
                else if( (op1->ttype == E_DOUBLE || op1->ttype == E_INT ) &&
                         (op2->ttype == E_DOUBLE || op2->ttype == E_INT )     )
                {
                    op1->data._double = toDouble(op1) * toDouble(op2);
                    op1->ttype = E_DOUBLE;
                }
                else
                {
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                    if( toDouble(op2) != 0.0 )
                    {
                    op1->data._double = toDouble(op1) / toDouble(op2);
                    op1->ttype = E_DOUBLE;
                    }
                    else
                    {
                        ERROR("Error on line %u: Unable to do operation [%s] with given operands. Divide with zero\n", op1->line, TOKEN_NAME[operation]);
                        free(op2);
                        return E_ZERO_DIV; 
                    }
                }
                else
                {
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                        {
                            int retval = lexsstrcmp(op1->data._string, op2->data._string, op1->length, op2->length );
                            val = ( retval < 0 ) ? true : false;
                            break;
                        }
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
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                        {
                            int retval = lexsstrcmp(op1->data._string, op2->data._string, op1->length, op2->length );
                            val = ( retval > 0 ) ? true : false;
                            break;
                        }
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
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                        {
                            int retval = lexsstrcmp(op1->data._string, op2->data._string, op1->length, op2->length );
                            val = ( retval <= 0 ) ? true : false;
                            break;
                        }
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
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                        {
                            int retval = lexsstrcmp(op1->data._string, op2->data._string, op1->length, op2->length );
                            val = ( retval >= 0 ) ? true : false;
                            break;
                        }
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
                    ERROR("Error on line %u: Unable to do operation [%s] with given operands.\n", op1->line, TOKEN_NAME[operation]);
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
                            if ( lexsstrcmp( op1->data._string, op2->data._string, op1->length, op2->length ) != 0 )
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

E_ERROR_TYPE assign(T_token *op1)
/* nastavuje globalnu premennu */
{
    PRINTD("%s()\n", __func__ );
    if ( op1 != NULL )
    {
        PRINTD("assigning, name %.1s\n", op1->data._string);
        bool dummy;
        if ( BTlookup(SwitchSTable , op1->data._string, op1->length, &assignvar, &dummy ) != E_OK )
            return E_INTERPRET_ERROR;
        PRINTD("name %.1s got id %d, defined = %d\n", op1->data._string, assignvar->offset, assignvar->assigned );
    }
    else
    {
        assignvar = NULL;
        PRINTD("preparing to emit return\n");
    }
        
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
    PRINTD("%s()\n", __func__ );
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
    PRINTD("%s()\n", __func__ );
    if((*ptr)->size <= (*ptr)->top)
    {
        PtrStack *tmp = *ptr;
        *ptr =  realloc (*ptr, sizeof( Instruction *) * ((*ptr)->size)*2 + sizeof( PtrStack ));
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
E_ERROR_TYPE MapTableInit( MapTable **ptr )
{
    PRINTD("%s()\n", __func__ );
    *ptr = malloc( sizeof( MapTable ) + FLEXIBLE_ARRAY_MEMBER * sizeof( int ) );
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
    PRINTD("%s()\n", __func__ );
    if((*ptr)->size <= (*ptr)->used_space)
    {
        MapTable *tmp = *ptr;
        *ptr =  realloc (*ptr, sizeof( int ) * ( ( *ptr )->size ) * 2 + sizeof( MapTable ) );
        if (*ptr == NULL) // ak realloc zlyha tak neuvolnuje pamat
        {
            free(tmp);
            return E_INTERPRET_ERROR;
        }
        if( tmp == Globalmap )
            Globalmap = *ptr;
        else
            Localmap = *ptr;
        (*ptr)->size = (*ptr)->size *2;
    }
    return E_OK;  
}

extern inline int lexsstrcmp( const char * str1, const char * str2, int str1_size, int str2_size )
{
    const char * offset;
    int result = 0;
   
    if ( str1_size > str2_size )
    {
        offset = str1 + str2_size;
        
        while ( ( result == 0 ) && ( str1 != offset ) )
        {
            result = ( unsigned ) *str1++ - ( unsigned ) *str2++;
        }
       
        if ( result == 0 )
            return 1;
    }
    else if ( str1_size < str2_size )
    {
        offset = str1 + str1_size;
       
        while ( ( result == 0 ) && ( str1 != offset ) )
        {
            result = ( unsigned ) *str1++ - ( unsigned ) *str2++;
        }
       
        if ( result == 0 )
            return -1;
    }
    else
    {
        offset = str1 + str1_size;
       
        while ( ( result == 0 ) && ( str1 != offset ) )
        {
            result = ( unsigned ) *str1++ - ( unsigned ) *str2++;
        }
    }
 
    return result;
}

void FindUnknownFunctions(FTableNode *ptr)
{
    PRINTD("%s()\n", __func__ );
    if( ptr != NULL )
    {
        if( ptr->metadata.state == E_UNKNOWN )
        {
            ERROR("Error on line %u: Function '", ptr->metadata.first_line);
            print_char(stderr, ptr->metadata.name, ptr->metadata.name_size );
            ERROR("' called but never defined.\n");;
        }
        FindUnknownFunctions(ptr->lptr);
        FindUnknownFunctions(ptr->rptr);
    }
}


