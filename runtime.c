#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"
#include "runtime.h"
#include "debug.h"
#include "built-in.h"

#define DEBUG

Stack stack;
T_DVAR retval;
Context* top;

const unsigned int MALLOC_SIZE = 32;

static inline E_ERROR_TYPE StackCheck()
{
    if ( (stack.top+1) < stack.size ) // prazdny zasobnik ma hodnotu top -1
    {   
        return E_OK;
    }
    else
    {
        // realloc
        Context **tmp = stack.array;
        stack.array = realloc( stack.array, sizeof( Context * ) * stack.size *2 );
        if (stack.array == NULL)
        {   
            stack.array = tmp;
            return E_INTERPRET_ERROR;
        }
        stack.size  *= 2;
    }
    return E_OK;
}

static inline E_ERROR_TYPE StackInit()
{
    stack.top = -1;
    stack.array = malloc( MALLOC_SIZE * sizeof(Instruction*) );
    if ( stack.array == NULL )
    {
        ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
        return E_INTERPRET_ERROR;
    }
    stack.size = MALLOC_SIZE;
    return E_OK;
}

static inline bool EndofProgram()
{
    return ( stack.top == -1 ) ? true : false;
}

static inline E_ERROR_TYPE AddFrame( unsigned int size )
{
    
    if ( StackCheck() != E_OK )
    {
        return E_INTERPRET_ERROR;
    }
    Context * tmp = malloc ( sizeof( Context ) +  sizeof(T_DVAR) * size );
    if ( tmp == NULL )
    {
        ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
        return E_INTERPRET_ERROR;
    }
    
    top = tmp;
    stack.top++;
    stack.array[stack.top] = tmp;
    tmp->size = size;
    for( unsigned int i = 0; i< size; i++ )
    {
        tmp->local[i].type = VAR_UNDEF;
        //tmp->local[i].size = 0;
    }
    return E_OK;
}

void print_local_var()
{
    for(unsigned int i = 0; i < top->size; i++)
    {
        printf( "[%u > ", i );
        print_DVAR( &top->local[i] );
        printf( "------------------\n" );
    }
}

void RuntimeErrorCleanup(void)
{
    if ( retval.type == VAR_STRING )
    {
        free( retval.data._string );
        retval.type = VAR_UNDEF;
    }
    Context *context;
    if( stack.array )
    {
        for( int i = 0; i <= stack.top ; i++ )
        {
            context = stack.array[i];
            for( unsigned int i = 0; i < context->size; i++ )
            {
                if( context->local[i].type == VAR_STRING )
                {
                    free( context->local[i].data._string );
                }
            }
            free( context );
        }
        free( stack.array );
        stack.array = NULL;
    }
}

E_ERROR_TYPE InterpretCode( Instruction *EntryPoint )
{
    if ( StackInit() != E_OK )
    {
        return E_OK;
    }
    retval.type = VAR_UNDEF; // navratovy register
    
    Instruction *EIP =  EntryPoint;
    
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
    
    while(1)
    {
        /*printf("EIP = %p\n", (void*) EIP);*/
        switch( EIP->opcode )
        {
            case MOV:
            {
                dest = EIP->attr.tac.dest;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &top->local[op1];
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
                
                if( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                    top->local[dest].type = VAR_UNDEF;
                }
                
                if( ptr1->type == VAR_STRING )
                {
                    top->local[dest].data._string = malloc( ptr1->size );
                    if( top->local[dest].data._string == NULL )
                    {
                        ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
                        return E_INTERPRET_ERROR;
                    }
                    memcpy( top->local[dest].data._string, ptr1->data._string, ptr1->size );
                    top->local[dest].type = VAR_STRING;
                    top->local[dest].size = ptr1->size;
                }
                else
                {
                    top->local[dest] = *ptr1;
                }                
                break;
            }
            case PLUS:
            {                
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &top->local[op2];
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
            if ( top->local[dest].type == VAR_STRING )
            {
                free( top->local[dest].data._string );
            }
            top->local[dest] = temp;
            break;  
            }
            
            case MINUS:
            {                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &top->local[op2];
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
            if ( top->local[dest].type == VAR_STRING )
            {
                free( top->local[dest].data._string );
            }
            top->local[dest] = temp;
            break;  
            }
            
            case MUL:
            {                
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
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
                
            if ( top->local[dest].type == VAR_STRING )
            {
                free( top->local[dest].data._string );
            }
            top->local[dest] = temp;
            break;  
            }
            
            case DIV:
            {                
                dest = EIP->attr.tac.dest;
                op1 = EIP->attr.tac.op1.data.offset;
                op2 = EIP->attr.tac.op2.data.offset;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
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
                
            if ( top->local[dest].type == VAR_STRING )
            {
                free( top->local[dest].data._string );
            }
            top->local[dest] = temp;
            break;  
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
                break;
            }
            
            case EQUAL:
            {
                T_DVAR temp;
                temp.type = VAR_BOOL;
                
                if ( EIP->attr.tac.op1.type == VAR_LOCAL )
                {
                    op1 = EIP->attr.tac.op1.data.offset;
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    op2 = EIP->attr.tac.op2.data.offset;
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ( ptr1->type == ptr2->type ) || 
                     ( ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) &&
                       ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING ) ) ) 
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
                        default:
                            ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                            return E_UNDEF_VAR;
                    }
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
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
                    ptr1 = &top->local[op1];
                }
                else
                {
                    ptr1 = &EIP->attr.tac.op1;
                }
                if ( EIP->attr.tac.op2.type == VAR_LOCAL )
                {
                    ptr2 = &top->local[op2];
                }
                else
                {
                    ptr2 = &EIP->attr.tac.op2;
                }
                
                if ( ptr1->type == VAR_CONSTSTRING || ptr1->type == VAR_STRING ) 
                {
                    if ( ptr2->type == VAR_CONSTSTRING || ptr2->type == VAR_STRING )
                    {
                        if ( ptr1->size + ptr2->size > 0 )
                        {
                            temp.data._string = malloc( ptr1->size + ptr2->size );
                            if ( temp.data._string == NULL )
                            {
                                ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
                                return E_INTERPRET_ERROR;
                            }
                            temp.size = ptr1->size + ptr2->size;
                            memcpy( temp.data._string, ptr1->data._string, ptr1->size );
                            memcpy( temp.data._string + ptr1->size, ptr2->data._string, ptr2->size );
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
                                ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
                                free(str.data._string);
                                return E_INTERPRET_ERROR;
                            }
                            
                            temp.size = ptr1->size + str.size;
                            memcpy( temp.data._string, ptr1->data._string, ptr1->size );
                            memcpy( temp.data._string + ptr1->size, str.data._string, str.size );
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
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = temp;
                break;
            }
            
            case JMP:
                EIP = EIP->attr.jump.jmp;
                continue;
                break;
            case COND:
                {
                    if( EIP->attr.jump.op1.type == VAR_LOCAL )
                    {
                        ptr1 = &top->local[EIP->attr.jump.op1.data.offset];
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
                    ptr1 = &(stack.array[stack.top-1])->local[EIP->attr.tac.op1.data.offset];
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
                top->local[EIP->attr.tac.dest] = *ptr1;
                if ( ptr1->type == VAR_STRING )
                {
                    top->local[EIP->attr.tac.dest].type = VAR_CONSTSTRING;
                }
                break;
            }
            case CALL:
                top->EIP = EIP->next;
                top->dest = EIP->attr.jump.dest;
                EIP = EIP->attr.jump.jmp;
                continue;
                break;

                
            case RET:
                
                if( EIP->attr.jump.op1.type == VAR_LOCAL )
                {
                    ptr1 = &top->local[EIP->attr.jump.op1.data.offset];
                    if( ptr1->type == VAR_UNDEF )
                    {
                        ERROR("runtime.c:%lu: Runtime error: Variable used, but undefined.\n", __LINE__ );
                        return E_UNDEF_VAR;
                    }
                    retval = *ptr1;
                    if ( retval.type == VAR_STRING || retval.type == VAR_CONSTSTRING )
                    {
                        if ( retval.size > 0 )
                        {
                            char *tmp = malloc( retval.size );
                            if ( tmp == NULL )
                            {
                                retval.type = VAR_UNDEF; // zneplatnim aby sa uvolnil len raz cez kontrolu ramcov
                                ERROR(" Interpret error: malloc() failed on line %lu, stack top %d.\n", __LINE__, stack.top );
                                return E_INTERPRET_ERROR;
                            }
                            retval.type = VAR_STRING;
                            memcpy( tmp, retval.data._string, retval.size );
                            retval.data._string = tmp;
                        }
                    }
                }
                else
                {
                    ptr1 = &EIP->attr.jump.op1;
                    retval = *ptr1;
                }
               
                
                for( unsigned int i = 0; i < top->size; i++ )
                {
                    if( top->local[i].type == VAR_STRING )
                    {
                        free( top->local[i].data._string );
                    }
                }
                dest = top->dest;
                EIP = top->EIP;
                free(top);
                --stack.top;
                if( EndofProgram() )
                {
                    if( retval.type == VAR_STRING )
                    {
                        free( retval.data._string );
                    }
                    free(stack.array);
                    return E_OK;
                }
                top = stack.array[stack.top];
                
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = retval;
                retval.type = VAR_UNDEF;
                continue;
                break;
            
            case CALL_BUILTIN:
            {
                E_ERROR_TYPE ret;
                
                if ( ( ret = EIP->attr.builtin.func( top->local, top->size, &retval ) ) != E_OK )
                {
                    retval.type = VAR_UNDEF;
                    ERROR("Runtime error: Built-in function failed.\n");
                    return ret;
                }
                
                for( unsigned int i = 0; i < top->size; i++ )
                {
                    if( top->local[i].type == VAR_STRING )
                    {
                        free( top->local[i].data._string );
                    }
                }
                free(top);
                --stack.top;
                top = stack.array[stack.top];
                dest = EIP->attr.builtin.dest;
                if ( top->local[dest].type == VAR_STRING )
                {
                    free( top->local[dest].data._string );
                }
                top->local[dest] = retval;
                retval.type = VAR_UNDEF;
                
                break;
            }
            default:
                ERROR("Runtime error: Unexpected instrunction with id %d.\n", EIP->opcode);
                return E_INTERPRET_ERROR;
            break;   
        }
        
        /*printf("------------------------------------------\n");
        print_local_var();
        printf("------------------------------------------\n");*/

        EIP = EIP->next;
    }
    return E_OK;
    
}