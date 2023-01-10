#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



/**********************************
** Global data
***********************************/
#define SIZE           ( sizeof(list)/sizeof(*list) )
int     list[]       = {5, 10, 17, 1, 16, 2, 0, -7, 4, 13, 6};
int     result[SIZE] = {0};



/**********************************
** Thread entry points and parameter definitions
***********************************/
typedef struct
{
  int *        subArray;
  unsigned int size;
} SortingThreadParameters;

typedef struct
{
  SortingThreadParameters left;
  SortingThreadParameters right;
} MergingThreadParameters;

void * sorter(void * params);	// thread that performs basic sorting algorithm
void * merger(void * params);	// thread that performs merging of results






int main (void)
{
  /**********************************
  ** Break the list of numbers to be sorted into two sub-arrays of (almost)
  ** equal size.  Have each half sorted in-place as separate threads
  ***********************************/
  enum            {LEFT, RIGHT};
  pthread_t       workers[2];  // indexed by LEFT or RIGHT

  // establish the first sorting thread
  // NOTE: The parameter object cannot be a stack object because threads have their own stack.
  //       If the parameter were a stack object, it may go out of scope (this function returns)
  //       before the child thread reads the contents.
  SortingThreadParameters * paramsLeft = malloc( sizeof( SortingThreadParameters ) );
  paramsLeft->subArray                 = list;
  paramsLeft->size                     = SIZE/2;

  // Create the first (left side) sorting thread
  pthread_attr_t  attr;
  pthread_attr_init(&attr);   // Set the default attribute values. No changes to default needed here.
  pthread_create( &workers[LEFT], &attr, sorter, paramsLeft );



  // establish the second sorting thread
  SortingThreadParameters * paramsRight = malloc( sizeof( SortingThreadParameters ) );
  paramsRight->subArray                 = list + paramsLeft->size;
  paramsRight->size                     = SIZE - paramsLeft->size;  // taking difference addresses an add number of elements

  // Create the second (right side) sorting thread
  pthread_create( &workers[RIGHT], NULL, sorter, paramsRight );       // NULL pointer attribute argument means to use default values (same net affect as above, just another way to get there)


  // now wait for the 2 sorting threads to finish
  pthread_join( workers[LEFT], NULL );  // NULL pointer means we are ignoring the returned valued from sorter()
  pthread_join( workers[RIGHT], NULL );




  /**********************************
  ** Now create a new thread to merge the two sorted halves into a single sorted
  ** result
  ***********************************/
  // establish the merge thread
  MergingThreadParameters * paramsMerge = malloc( sizeof( MergingThreadParameters ) );
  paramsMerge->left  = *paramsLeft;
  paramsMerge->right = *paramsRight;

  pthread_t tid;
  pthread_create( &tid, NULL, merger, paramsMerge );

  /* wait for the merge thread to finish */
  pthread_join( tid, NULL );


  // free parameter storage
  free( paramsLeft  );
  free( paramsRight );
  free( paramsMerge );

  /**********************************
  ** At this point data has been completely sorted and available in the results array.
  ** Print the sorted data and end the program
  ***********************************/
  // output the sorted array
  for (size_t i = 0; i < SIZE; i++) printf("%d  ",result[i]);
  printf("\n");

  return 0;
}










/**********************************
** Sorting thread.
**
** This thread can essentially use any algorithm for sorting
***********************************/

void *sorter(void *params)
{
  SortingThreadParameters * p = params;

  #if 1  // You can use the suggested sorting algorithm below or write your own
    // Simple, but inefficient bubble sort.
    int * const begin = p->subArray;
    int * const end   = begin + p->size;

    for( int * i = begin; i < end; ++i ) for( int * j = i+1; j < end; ++j )
    {
      if(*i > *j) // swap?
      {
        int temp = *i;
        *i       = *j;
        *j       = temp;
      }
    }
  #endif

  pthread_exit(0);
}










/**********************************
** Merge thread
**
** Uses simple merge sort for merging two sublists
***********************************/

void *merger(void *params)
{
  MergingThreadParameters * p = params;

  #if 1    // You can use the suggested merge algorithm below or write your own
    int * const leftBegin  = p->left .subArray;
    int * const rightBegin = p->right.subArray;
    int * const leftEnd    = leftBegin  + p->left .size;
    int * const rightEnd   = rightBegin + p->right.size;

    int * resultPosition  = result;	// position being inserted into result list

    int *leftPosition  = leftBegin,
        *rightPosition = rightBegin;

    while (leftPosition < leftEnd  &&  rightPosition < rightEnd)
    {
      if (*leftPosition < *rightPosition) *resultPosition++ = *leftPosition++;
      else                                *resultPosition++ = *rightPosition++;
    }

    // copy the remainder
    // Note: only one of these loops will execute.
    while (leftPosition  < leftEnd)  *resultPosition++ = *leftPosition++;
    while (rightPosition < rightEnd) *resultPosition++ = *rightPosition++;
  #endif

  pthread_exit(0);
}
