#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"
#include "runtime.h"
#include "debug.h"
#include "built-in.h"

#define DEBUG

Stack* stack;
T_DVAR retval;
T_DVAR* local;
T_DVAR* prev_local;
Vector* actual_bucket;
Instruction *EIP;

const unsigned int BUCKET_INIT = 5;
const unsigned int ARRAY_SIZE = 1500;

static inline E_ERROR_TYPE StackCheck()
{
    if( ( stack->actual + 1 ) >= ( stack->size ) )
    {
        //printf("realokuje, %d", actual_bucket->used);
        /* treba realokovat */
        Stack* tmp = stack;
        stack = realloc( stack, sizeof( Stack ) + sizeof( Vector * ) * 2 * stack->size );
        if ( stack == NULL )
        {
            stack = tmp;
            return E_INTERPRET_ERROR;
        }
        //actual_bucket = stack->bucket[stack->actual];
        memset( &(stack->bucket[stack->size]), 0,  sizeof( Vector * ) * stack->size );
        stack->size = stack->size *2;
    }
    return E_OK;
}

static inline E_ERROR_TYPE StackInit()
{
    stack = malloc( sizeof( Stack ) + sizeof( Vector * ) * BUCKET_INIT );
    if( stack == NULL )
    {
        ERROR(" Interpret error: malloc() failed on line %ld.\n", __LINE__ );
        return E_INTERPRET_ERROR;
    }
    memset( stack, 0, sizeof( Stack ) + sizeof( Vector * ) * BUCKET_INIT );
    for( unsigned int i = 0; i < BUCKET_INIT; i++  )
    {
        stack->bucket[i] = malloc( ARRAY_SIZE * sizeof( T_DVAR ) + sizeof( Vector ) );
        if( stack->bucket[i] == NULL )
        {
            ERROR(" Interpret error: malloc() failed on line %ld.\n", __LINE__ );
            return E_INTERPRET_ERROR;
        }
        memset( stack->bucket[i], 0, ARRAY_SIZE * sizeof( T_DVAR ) + sizeof( Vector ) );
    }
    stack->size  = BUCKET_INIT;
    // stack->actual = 0;    // uz je spravene memsetom
    actual_bucket = stack->bucket[0];
    //memset( actual_bucket, 0, ARRAY_SIZE * sizeof( T_DVAR ) + sizeof( Vector ) );
    local = (void*)actual_bucket;
    
    return E_OK;
}

static inline bool EndofProgram()
{
    return ( (void *)local == (void* )actual_bucket ) ? true : false;  
}

static inline E_ERROR_TYPE AddFrame( unsigned int size )
{
   // printf("pridavam frame so size %d\n", size);
    T_DVAR *tmp;
    /* DEBUG */
    /*if ( ( size + 3 ) > ARRAY_SIZE )
        printf("zasobnik nie je dostatocne dlhy, nastavit premennu\n");*/
    
    if( ARRAY_SIZE >= ( size + actual_bucket->used + 3 ) )
    {
        /* mam mesto*/
        //printf("mam miesto v buckete %d/%d -> miesto %d/%d\n", stack->actual, stack->size-1, actual_bucket->used, ARRAY_SIZE);
        tmp = &(actual_bucket->local[actual_bucket->used]);
    }
    else
    {
        //printf("nemam miesto -> vyrobim [%d/%d -> miesto %d/%d\n", stack->actual, stack->size-1, actual_bucket->used, ARRAY_SIZE);
        /* nemam miesto */
        if( StackCheck() != E_OK )
        {
            return E_INTERPRET_ERROR;
        }
        /* uz mam miesto */
        stack->actual++;
        actual_bucket = stack->bucket[stack->actual];
        
        if ( actual_bucket == NULL )
        {
            //printf("vymallocujem novy bucket\n");
            actual_bucket = malloc( ARRAY_SIZE * sizeof( T_DVAR ) + sizeof( Vector ) );
            if ( actual_bucket == NULL )
            {
                return E_INTERPRET_ERROR;
            }
            actual_bucket->used = 0;
            stack->bucket[stack->actual] = actual_bucket;
        }
        //printf("tu mam miesto %d/%d -> miesto %d/%d\n", stack->actual, stack->size-1, actual_bucket->used, ARRAY_SIZE);
        tmp = &(actual_bucket->local[actual_bucket->used]);
    }
    /* EIP este neviem*/
    tmp->type = VAR_NO_VAR;
                                                                //tmp->data.EIP = EIP;
    tmp++;
    /* DEST neviem este + prev_local viem */
    tmp->type = VAR_NO_VAR;
    tmp->data.prev_local = local;
    tmp++;
    /* string counter a size ramca */
    tmp->type = VAR_NO_VAR;
    tmp->size = size;
    tmp->data.offset = 0; // nula
    tmp++;
    /* zmenim prev local a local */
    prev_local = local;
    local = tmp;
    actual_bucket->used = actual_bucket->used + ( size + 3 );
    memset( local, 0, size * sizeof( T_DVAR ) );
    //printf("opustam addframe %d/%d -> miesto %d/%d\n", stack->actual, stack->size-1, actual_bucket->used, ARRAY_SIZE);
    return E_OK;
}

void print_local_var()
{
    for(unsigned int i = 0; i < local[-1].size; i++)
    {
        printf( "[%u > ", i );
        print_DVAR( &local[i] );
        printf( "------------------\n" );
    }
}

void RuntimeErrorCleanup(void)
{
    if ( retval.type == VAR_STRING )
    {
        free( retval.data._string );
    }
    Vector *ptr;
    if( stack != NULL )
    {
        for( int i = 0; i <= stack->size ; i++ )
        {
            ptr = stack->bucket[i];
            if ( ptr != NULL )
            {
                for( unsigned int i = 0; i < ptr->used; i++ )
                {
                    if( ptr->local[i].type == VAR_STRING )
                    {
                        free( ptr->local[i].data._string );
                    }
                }
                free( ptr );
            }
        }
        free( stack );
    }
}

E_ERROR_TYPE InterpretCode( Instruction *EntryPoint )
{
    if ( StackInit() != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    retval.type = VAR_UNDEF; // navratovy register
    
    EIP =  EntryPoint;
    
    /* prva instrukcia je vzdy START */
    if( AddFrame( EntryPoint->attr.size ) != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    EIP = EntryPoint->next;
    
    unsigned int dest;
    unsigned int op1;
    unsigned int op2;
    
    T_DVAR* ptr1;
    T_DVAR* ptr2;
    
    while( 1 )
    {   
        switch( EIP->opcode )
        {
            case MOV:
            {
                dest = EIP->attr.tac.dest;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &local[op1];
                    if (ptr1->type == VAR_UNDEF )
                    {
                        ERROR("Runtime error: Using undefined variable.\n");
                        return E_UNDEF_VAR;
                    }
                    if ( op1 == dest )
                        break;              // $a = $a;
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                
                if( local[dest].type == VAR_STRING )
                {
                    local[-1].data.offset--;
                    free( local[dest].data._string );
                    local[dest].type = VAR_UNDEF;
                }
                
                if( ptr1->type == VAR_STRING )
                {
                    local[dest].data._string = malloc( ptr1->size );
                    if( local[dest].data._string == NULL )
                    {
                        ERROR(" Interpret error: malloc() failed on line %lu.\n", __LINE__ );
                        return E_INTERPRET_ERROR;
                    }
                    memcpy( local[dest].data._string, ptr1->data._string, ptr1->size );
                    local[dest].type = VAR_STRING;
                    local[dest].size = ptr1->size;
                    local[-1].data.offset++;
                }
                else
                {
                    local[dest] = *ptr1;
                }                
                break;
            }
            case RET:
            {
                if( EIP->attr.jump.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[EIP->attr.jump.op1.data.offset];
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    retval = *ptr1;
                    if ( retval.type > VAR_STRING )
                    {
                        if ( retval.type == VAR_CONSTSTRING )
                        {
                            if( retval.size > 0 )
                            {
                                char *tmp = malloc( retval.size );
                                if ( tmp == NULL )
                                {
                                    retval.type = VAR_UNDEF; // zneplatnim aby sa uvolnil len raz cez kontrolu ramcov
                                    ERROR(" Interpret error: malloc() failed on line %lu.\n", __LINE__ );
                                    return E_INTERPRET_ERROR;
                                }
                                retval.type = VAR_STRING;
                                memcpy( tmp, retval.data._string, retval.size );
                                retval.data._string = tmp;
                            }
                        }
                        else
                        {
                            local[-1].data.offset--;
                            ptr1->type = VAR_UNDEF;
                        }
                    }
                }
                else
                {
                    ptr1 = &EIP->attr.jump.op1;
                    retval = *ptr1;
                }
               
                if ( local[-1].data.offset > 0 )
                {
                    //printf("SPUSTAM ODMAZAVANIE\n");
                    for( unsigned int i = 0; i < local[-1].size; i++ )
                    {
                        if( local[i].type == VAR_STRING )
                        {
                            free( local[i].data._string );
                        }
                    }
                }
                dest = local[-2].size;
                EIP = local[-3].data.EIP;
                actual_bucket->used = actual_bucket->used - local[-1].size -3;

                local = local[-2].data.prev_local; // predosly lokal
                
                if( EndofProgram() )
                {
                    if( retval.type == VAR_STRING )
                    {
                        free( retval.data._string );
                    }
                    /* uvolnim zasobniky */
                    for(int i = 0; i < stack->size; i++ )
                    {
                        if( stack->bucket[i] != NULL )
                        {
                            free( stack->bucket[i] );
                        }
                    }
                    free( stack );
                    stack = NULL;
                    return E_OK;
                }
                //printf("NOT EOP!!\n");
                /*  */
                if ( actual_bucket->used == 0 )
                {
                    stack->actual--;
                    actual_bucket = stack->bucket[stack->actual];
                }
                
                if ( local[dest].type == VAR_STRING )
                {
                    local[-1].data.offset--;
                    free( local[dest].data._string );
                }
                if ( retval.type == VAR_STRING )
                {
                    local[-1].data.offset++;
                }
                local[dest] = retval;
                retval.type = VAR_UNDEF;
                continue;
            }
            case LESS:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) < ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) < ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) < ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval < 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = false;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval < 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime Error: Incompatible types for operation <.\n");
                    return E_INCOMPATIBLE;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case COND:
            {
                if( EIP->attr.jump.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[EIP->attr.jump.op1.data.offset];
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                }
                else
                {
                    ptr1 = &EIP->attr.jump.op1;
                }
                
                switch ( ptr1->type )
                {
                    case VAR_BOOL:
                        if ( ptr1->data._bool == false )
                        {
                            EIP = EIP->attr.jump.jmp;
                            continue;
                        }
                        break;
                    case VAR_INT:
                        if ( ptr1->data._int == 0 )
                        {
                            EIP = EIP->attr.jump.jmp;
                            continue;
                        }
                        break;
                    case VAR_STRING:
                    case VAR_CONSTSTRING:
                        if ( ptr1->size == 0 )
                        {
                            EIP = EIP->attr.jump.jmp;
                            continue;
                        }
                        break;
                    case VAR_DOUBLE:
                        if ( ptr1->data._double == 0.0 )
                        {
                            EIP = EIP->attr.jump.jmp;
                            continue;
                        }
                        break;
                    case VAR_NULL:
                        {
                            EIP = EIP->attr.jump.jmp;
                            continue;
                            break;
                        }
                    default:
                        break;
                }
                break;   
            }
            case INC:
            {
                dest = EIP->attr.tac.dest;
                
                if( local[dest].type == VAR_INT )
                {
                    local[dest].data._int += 1;
                }
                else if( local[dest].type == VAR_DOUBLE )
                {
                    local[dest].data._double += 1;
                }
                else if(local[dest].type == VAR_UNDEF )
                {
                    ERROR("runtime.c:%lu: Runtime error: Cannot increment undefined variable.\n", __LINE__ );
                    return E_UNDEF_VAR;
                }
                else
                {
                    ERROR("runtime.c:%lu: Runtime error: Incompatible type to incremet.\n", __LINE__ );
                    return E_INCOMPATIBLE;
                }
                break;
            }
            case DEC:
            {
                dest = EIP->attr.tac.dest;
                
                if( local[dest].type == VAR_INT )
                {
                    local[dest].data._int -= 1;
                }
                else if( local[dest].type == VAR_DOUBLE )
                {
                    local[dest].data._double -= 1;
                }
                else if( local[dest].type == VAR_UNDEF )
                {
                    ERROR("runtime.c:%lu: Runtime error: Cannot decrement undefined variable.\n", __LINE__ );
                    return E_UNDEF_VAR;
                }
                else
                {
                    ERROR("runtime.c:%lu: Runtime error: Incompatible type to decremet.\n", __LINE__ );
                    return E_INCOMPATIBLE;
                }
                break;
            }
            case PLUS:
            {                
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[EIP->attr.tac.op1.data.offset];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[EIP->attr.tac.op2.data.offset];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                T_DVAR temp;
                
                if( ptr1->type == VAR_INT )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._int = ptr1->data._int + ptr2->data._int;
                        temp.type = VAR_INT;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._int + ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [+] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else if( ptr1->type == VAR_DOUBLE )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._double = ptr1->data._double + (double)ptr2->data._int;
                        temp.type = VAR_DOUBLE;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._double + ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [+] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime error: Unsupported operation [+] with given operands.\n");
                    return E_INCOMPATIBLE;
                } 
                
                dest = EIP->attr.tac.dest;    
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;  
            }
            case MINUS:
            {                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                T_DVAR temp;
                
                if( ptr1->type == VAR_INT )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._int = ptr1->data._int - ptr2->data._int;
                        temp.type = VAR_INT;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._int - ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [-] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else if( ptr1->type == VAR_DOUBLE )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._double = ptr1->data._double - (double)ptr2->data._int;
                        temp.type = VAR_DOUBLE;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._double - ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [-] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime error: Unsupported operation [-] with given operands.\n");
                    return E_INCOMPATIBLE;
                } 
                dest = EIP->attr.tac.dest;    
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;  
            }
            case MUL:
            {                
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                T_DVAR temp;
                
                if( ptr1->type == VAR_INT )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._int = ptr1->data._int * ptr2->data._int;
                        temp.type = VAR_INT;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._int * ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [*] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else if( ptr1->type == VAR_DOUBLE )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        temp.data._double = ptr1->data._double * (double)ptr2->data._int;
                        temp.type = VAR_DOUBLE;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        temp.data._double = (double)ptr1->data._double * ptr2->data._double;
                        temp.type = VAR_DOUBLE;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [*] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime error: Unsupported operation [*] with given operands.\n");
                    return E_INCOMPATIBLE;
                } 
                
            if ( local[dest].type == VAR_STRING )
            {
                free( local[dest].data._string );
                local[-1].data.offset--;
            }
            local[dest] = temp;
            break;  
            }
            case DIV:
            {                
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                T_DVAR temp;
                temp.type = VAR_DOUBLE;
                if( ptr1->type == VAR_INT )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        if( ptr2->data._int == 0)
                        {
                            ERROR("runtime.c:%lu: Runtime error: Divide with zero.\n", __LINE__ );
                            return E_ZERO_DIV;
                        }
                        temp.data._double = (double)ptr1->data._int / (double)ptr2->data._int;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        if( ptr2->data._double == 0)
                        {
                            ERROR("runtime.c:%lu: Runtime error: Divide with zero.\n", __LINE__ );
                            return E_ZERO_DIV;
                        }
                        temp.data._double = (double)ptr1->data._int / ptr2->data._double;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [/] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else if( ptr1->type == VAR_DOUBLE )
                {
                    if ( ptr2->type == VAR_INT )
                    {
                        if( ptr2->data._int == 0)
                        {
                            ERROR("runtime.c:%lu: Runtime error: Divide with zero.\n", __LINE__ );
                            return E_ZERO_DIV;
                        }
                        temp.data._double = ptr1->data._double / (double)ptr2->data._int;
                    }
                    else if ( ptr2->type == VAR_DOUBLE )
                    {
                        if( ptr2->data._double == 0)
                        {
                            ERROR("runtime.c:%lu: Runtime error: Divide with zero.\n", __LINE__ );
                            return E_ZERO_DIV;
                        }
                        temp.data._double = (double)ptr1->data._double / ptr2->data._double;
                    }
                    else
                    {
                        if( ptr2->type == VAR_UNDEF )
                        {
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                        }
                        ERROR("Runtime error: Unsupported operation [/] with given operands.\n");
                        return E_INCOMPATIBLE;
                    } 
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime error: Unsupported operation [/] with given operands.\n");
                    return E_INCOMPATIBLE;
                } 
                
            if ( local[dest].type == VAR_STRING )
            {
                free( local[dest].data._string );
                local[-1].data.offset--;
            }
            local[dest] = temp;
            break;  
            }
            case JMP:
                EIP = EIP->attr.jump.jmp;
                continue;
                break;
            case CREATE:
            {
                if( AddFrame( EIP->attr.size ) != E_OK )
                {
                    return E_INTERPRET_ERROR;
                }
                break;
            }
            case PUSH:
            {
                if( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &prev_local[EIP->attr.tac.op1.data.offset];
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                local[EIP->attr.tac.dest] = *ptr1;
                if ( ptr1->type == VAR_STRING )
                {
                    local[EIP->attr.tac.dest].type = VAR_CONSTSTRING;
                }
                break;
            }
            case CALL:
                local[-3].data.EIP = EIP->next;
                local[-2].size = EIP->attr.jump.dest;
                EIP = EIP->attr.jump.jmp;
                continue;
                break;
            case CALL_BUILTIN:
            {
                E_ERROR_TYPE ret;
                
                if ( ( ret = EIP->attr.builtin.func( local, local[-1].size, &retval ) ) != E_OK )
                {
                    retval.type = VAR_UNDEF;
                    ERROR("Runtime error: Built-in function failed.\n");
                    return ret;
                }
                
                /*for( unsigned int i = 0; i < top->size; i++ )
                {
                    if( top->local[i].type == VAR_STRING )
                    {
                        free( top->local[i].data._string );
                    }
                }*/
                
                actual_bucket->used = actual_bucket->used - local[-1].size -3;
                local = local[-2].data.prev_local;
                dest = EIP->attr.builtin.dest;
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                if ( retval.type == VAR_STRING )
                {
                    local[-1].data.offset++;
                }
                local[dest] = retval;
                retval.type = VAR_UNDEF;
                break;
            }
            case GREATER:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) > ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) > ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) > ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval > 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = false;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval > 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime Error: Incompatible types for operation >.\n");
                    return E_INCOMPATIBLE;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case LESSEQ:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) <= ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) <= ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) <= ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval <= 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = true;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval <= 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime Error: Incompatible types for operation <=.\n");
                    return E_INCOMPATIBLE;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case GREATEREQ:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) >= ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) >= ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) >= ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval >= 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = true;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval >= 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime Error: Incompatible types for operation >=.\n");
                    return E_INCOMPATIBLE;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case NONEQUAL:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) != ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) != ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) != ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string, ptr2->data._string, ptr1->size, ptr2->size );
                                temp.data._bool = ( retval != 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = false;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval != 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    temp.data._bool = true;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            
            case EQUAL:
            {
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) ) 
                {
                    switch( ptr1->type )
                    {
                        case VAR_INT:
                            temp.data._bool = ( ptr1->data._int ) == ( ptr2->data._int );
                            break;
                        case VAR_DOUBLE:
                            temp.data._bool = ( ptr1->data._double ) == ( ptr2->data._double );
                            break;
                        case VAR_BOOL:
                            temp.data._bool = ( ptr1->data._bool ) == ( ptr2->data._bool );
                            break;
                        case VAR_CONSTSTRING:
                        case VAR_STRING:
                            {
                                int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval == 0 )? true : false;
                            break;
                            }
                        case VAR_NULL:
                            temp.data._bool = true;
                            break;
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
                }
                else if ( ptr1->type >= VAR_STRING && ptr2->type >= VAR_STRING )
                {
                    int retval = lexsstrcmp( ptr1->data._string,ptr2->data._string, ptr1->size, ptr2->size );
                            temp.data._bool = ( retval == 0 )? true : false;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    temp.data._bool = false;
                }
                dest = EIP->attr.tac.dest;
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case CONCAT:
            {
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                T_DVAR temp;
                temp.type = VAR_STRING;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ptr1->type >= VAR_STRING ) 
                {
                    if ( ptr2->type >= VAR_STRING )
                    {
                        if ( ptr1->size + ptr2->size > 0 )
                        {
                            temp.data._string = malloc( ptr1->size + ptr2->size );
                            if ( temp.data._string == NULL )
                            {
                                ERROR(" Interpret error: malloc() failed on line %lu.\n", __LINE__ );
                                return E_INTERPRET_ERROR;
                            }
                            temp.size = ptr1->size + ptr2->size;
                            memcpy( temp.data._string, ptr1->data._string, ptr1->size );
                            memcpy( temp.data._string + ptr1->size, ptr2->data._string, ptr2->size );
                            local[-1].data.offset++;
                        }
                        else
                        {
                            temp.type = VAR_CONSTSTRING;
                            temp.size = 0;
                        }
                    }
                    else
                    { 
                        T_DVAR str;
                        E_ERROR_TYPE retval = strval( ptr2, 1, &str );
                        if ( retval != E_OK )
                        {
                            if( ptr2->type == VAR_UNDEF )
                            {
                                ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                                return E_UNDEF_VAR;
                            }
                            return retval;
                        }
                        if ( ptr1->size + str.size > 0 )
                        {
                            temp.data._string = malloc( ptr1->size + str.size );
                            if ( temp.data._string == NULL )
                            {
                                ERROR(" Interpret error: malloc() failed on line %lu.\n", __LINE__ );
                                free(str.data._string);
                                return E_INTERPRET_ERROR;
                            }
                            
                            temp.size = ptr1->size + str.size;
                            memcpy( temp.data._string, ptr1->data._string, ptr1->size );
                            memcpy( temp.data._string + ptr1->size, str.data._string, str.size );
                            local[-1].data.offset++;
                            if ( str.type != VAR_CONSTSTRING )
                            {
                                free(str.data._string);
                            }
                        }
                        else
                        {
                            temp.type = VAR_CONSTSTRING;
                            temp.size = 0;
                        }
                    }
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    ERROR("Runtime Error: Incompatible types for operation Concatenate.\n");
                    return E_OTHER;
                }
                if ( local[dest].type == VAR_STRING )
                {
                    local[-1].data.offset--;
                    free( local[dest].data._string );
                }
                local[dest] = temp;
                break;
            }
            case AND:
            {
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == VAR_BOOL ) && ( ptr2->type == VAR_BOOL ) ) 
                {
                    temp.data._bool = ptr1->data._bool && ptr2->data._bool;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    else
                    {
                        ERROR("Runtime Error: Incompatible types for operation AND.\n");
                    }
                }
                dest = EIP->attr.tac.dest;
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case OR:
            {
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == VAR_BOOL ) && ( ptr2->type == VAR_BOOL ) ) 
                {
                    temp.data._bool = ptr1->data._bool || ptr2->data._bool;
                }
                else
                {
                    if( ptr1->type == VAR_UNDEF || ptr2->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    else
                    {
                        ERROR("Runtime Error: Incompatible types for operation AND.\n");
                    }
                }
                dest = EIP->attr.tac.dest;
                if ( local[dest].type == VAR_STRING )
                {
                    free( local[dest].data._string );
                    local[-1].data.offset--;
                }
                local[dest] = temp;
                break;
            }
            case DUMMY:
                break;
            default:
                ERROR("Runtime error: Unexpected instrunction with id %d.\n", EIP->opcode);
                return E_INTERPRET_ERROR;
            break;
        }
        /*printf("------------------------------------------\n");
        print_local_var();
        printf("------------------------------------------\n");
        printf("actual bucket = %p %d\n", actual_bucket, actual_bucket->used);*/
        
        EIP = EIP->next;
    }
    return E_OK;
    
}