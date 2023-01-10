#include <stdlib.h>  // NULL, malloc(), free()
#include <assert.h>  // assert()

#include "stack.h"



/*******************************************************************************
** Public types and objects definitions
*******************************************************************************/
const Stack EmptyStack = {NULL, 0, PTHREAD_MUTEX_INITIALIZER};

struct StackNode
{
  value_t            _data;
  struct StackNode * _next;
};



/*******************************************************************************
** Private helper functions
*******************************************************************************/
static int isValid( Stack * stack )
{
  // verify stack really points to something, presumably a stack
  assert( stack );

  pthread_mutex_lock(&stack->lock);

  // Verify the number of nodes in the stack matches the stack's size
  unsigned count = stack->_size;

  for(StackNode * current = stack->_top;  current;  current = current->_next)  --count;

  pthread_mutex_unlock(&stack->lock);

  return count == 0;
}



/*******************************************************************************
** Public function definitions
*******************************************************************************/
void push( Stack * stack, value_t data )
{

  // To help detect stack integrity problems, verify consistency between "top" and "size" attributes
  assert( stack && isValid(stack) );

  // provision memory for a new node
  StackNode * newNode = malloc( sizeof(StackNode) );

  // populate the new node with data
  newNode->_data = data;
  
  pthread_mutex_lock(&stack->lock);

  // link the new node onto the top of the stack and advance the stack top
  newNode->_next = stack->_top;
  stack->_top    = newNode;

  // maintain stack size
  ++stack->_size;

  pthread_mutex_unlock(&stack->lock);
}



value_t pop( Stack * stack )
{

  // To help detect stack integrity problems, verify consistency between "top" and "size" attributes
  assert( stack && isValid(stack) );
  

  // What to do if asked to return data from an empty stack?
  if( isEmpty( stack ) ) // is empty?
  {
    return (value_t)0;
  }

  pthread_mutex_lock(&stack->lock);

  // advance the top of the stack to the next element
  StackNode * temp = stack->_top;
  stack->_top      = stack->_top->_next;

  // maintain stack size
  --stack->_size;

  pthread_mutex_unlock(&stack->lock);

  // hold a copy of the data at the top of the stack
  value_t data = temp->_data;

  // release the popped node
  free( temp );

  // return the requested data
  return data;
}



int isEmpty( Stack * stack )
{
  // To help detect stack integrity problems, verify consistency between "top" and "size" attributes
  assert( stack && isValid(stack) );

  // pthread_mutex_lock(&stack->lock);
  // if (stack->_top == NULL) {
  //   pthread_mutex_unlock(&stack->lock);
  //   return 1;
  // }
  // pthread_mutex_unlock(&stack->lock);
  // return 0;
  return stack->_top == NULL;
}
