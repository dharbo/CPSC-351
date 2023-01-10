#pragma once
#include <stdlib.h>  // NULL

/*******************************************************************************
** Public type declarations
*******************************************************************************/
typedef struct StackNode StackNode;  // Definition is private (this is called an incomplete type, or sometime called a forward definition)
typedef int    value_t;              // For simplicity, the stack will hold integers


/*******************************************************************************
** Public type definitions
** The Stack Abstract Data Type Definition
*******************************************************************************/
typedef struct
{
  StackNode * _top;
  unsigned    _size;
  pthread_mutex_t lock;
} Stack;


/*******************************************************************************
** Public function declarations
** Stack operations
*******************************************************************************/
void    push     ( Stack * stack, value_t data );
value_t pop      ( Stack * stack               );
int     isEmpty  ( Stack * stack               );


/*******************************************************************************
** Public object declarations
*******************************************************************************/
extern const Stack EmptyStack;