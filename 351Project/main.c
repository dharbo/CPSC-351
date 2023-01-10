#include <pthread.h> // pthread_t, pthread_create(), pthread_join
#include <stdint.h>  // int32_t, uintptr_t
#include <stdio.h>   // printf()
#include <stdlib.h>  // random_r(), srandom_r(), random_data
#include <time.h>    // time()

#include "stack.h"




/*******************************************************************************
** Random numbers are used in this C program.  See https://linux.die.net/man/3/random_r which reminds us that, on a POSIX compliant
** system, rand() has long been deprecated, random() and drand48() are (intentionally) not thread safe in favor of performance, and
** random_r() and drand48_r() are the reentrant, thread safe versions used in a multi-threaded program.  Single threaded programs
** may use the reentrant versions, but multi-threaded programs should avoid the non-reentrant versions.
**
** Neither of these should be used with C++, by the way. C++11 introduced a superior, object based solution.
*******************************************************************************/


/*******************************************************************************
** Private types and objects definitions
*******************************************************************************/
typedef struct
{
  Stack    * stacks;      // array of stacks
  unsigned   stacksSize;  // number of stacks in the array
} SomethingReallyImportant_Parameters;


/*******************************************************************************
** Private helper functions
*******************************************************************************/
// static void somethingReallyImportant(Stack stacks[], unsigned stacksSize)
static void * somethingReallyImportant(void * args)
{
  // Typecast parameters into something more understandable
  SomethingReallyImportant_Parameters * parameters = args;  // typecast into something more usable
  Stack * stacks      = parameters->stacks;
  unsigned stacksSize = parameters->stacksSize;


  // Seed for random generator with the address of a local stack object helping ensure each thread gets a unique seed
  struct random_data  randomNumberBuffer     = {0};
  char                randomNumberState[128] = {0};
  initstate_r( (uintptr_t)&randomNumberBuffer, randomNumberState, sizeof(randomNumberState), &randomNumberBuffer );

  // About two-thirds of the time push a random 12-bit number (0..4095) onto one of the stacks, and pop a value off
  // one of the stack about one-third of the time.
  int32_t theRandomNumber = 0;
  for (unsigned i = 0; i < 1000; ++i)
  {
    random_r( &randomNumberBuffer, &theRandomNumber );          // get the next random number
    Stack * stack = stacks + (theRandomNumber % stacksSize);    // pick one of the stacks

    if( theRandomNumber%3 == 0 )   pop ( stack );
    else                           push( stack, theRandomNumber % (1<<12) );
  }

  return NULL;
}



/*******************************************************************************
** Process entry point
*******************************************************************************/
int main(void)
{
  // create and populate an array of stacks just so we can randomly intermix invoking operations on them
  Stack          arrayOfStacks[] = {EmptyStack, EmptyStack, EmptyStack, EmptyStack};       // Food for thought:  How much data is really placed on the thread's call stack?
  unsigned const size            = sizeof(arrayOfStacks)/sizeof(*arrayOfStacks);

  // Create many threads all running concurrently.
  #define THREAD_COUNT 200u
  pthread_t thread_ids[ THREAD_COUNT ] = {0};

  // do something really important that uses all those stacks
  // somethingReallyImportant(arrayOfStacks, size);
  SomethingReallyImportant_Parameters arguments = {arrayOfStacks, size};
  for( unsigned i = 0;  i != THREAD_COUNT;  ++i )   pthread_create( thread_ids+i, NULL, somethingReallyImportant, &arguments );

  // Wait for all threads to complete
  for( unsigned i = 0;  i != THREAD_COUNT;  ++i )   pthread_join( thread_ids[i], NULL );



  // Display each stack's size and its first 5 elements
  for( unsigned i = 0; i<size; ++i )
  {
    Stack * stack = arrayOfStacks + i;
    printf("stack[%d] (%05d):  ", i, stack->_size);

    for(unsigned j = 0;  (j < 4) && (!isEmpty(stack));  ++j)   printf( "%5d, ", pop( stack ) );
    if( !isEmpty(stack) ) printf( "%5d\n", pop( stack ) );
  }

  printf("\nProgram completed successfully\n");
  return 0;
}
