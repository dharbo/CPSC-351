/************************************************************************************************************************************
** (C) Copyright 2015 by Thomas Bettens. All Rights Reserved.
**
** DISCLAIMER: The authors have used their best efforts in preparing this code. These efforts include the development, research, and
** testing of the theories and programs to determine their effectiveness. The authors make no warranty of any kind, expressed or
** implied, with regard to these programs or to the documentation contained within. The authors shall not be liable in any event for
** incidental or consequential damages in connection with, or arising out of, the furnishing, performance, or use of these libraries
** and programs.  Distribution without written consent from the authors is prohibited.
**
** Last modified:  09-JUL-2019
** Last verified:  09-JUL-2019
** Verified with:  gcc version 9.1.1 20190611 [gcc-9-branch revision 272147] (SUSE Linux product:openSUSE = 20190704-0)
************************************************************************************************************************************/

#include <stddef.h>       // NULL
#include <stdlib.h>       // malloc(), free()
#include <stdio.h>        // printf(), fprintf(), scanf(), fscanf(), stderr, stdin, stdout
#include <string.h>       // strlen(), memcpy()
#include <unistd.h>       // pipe(), close(), fork(), write(), read()

#include "performParentTask.h"



/******************************************************************************
** PRIVATE TYPES and DATA
******************************************************************************/
// none

/******************************************************************************
** Function definitions
******************************************************************************/
void performParentTask( const int writeEndFileDescriptor )
{
                           // These string literals (separated only by white space) are concatenated together by the preprocessor to form one long string
  const char write_msg[] = "Hello world! CPSC 351 \"Operating System Concepts\" Rocks!\n"
                           "My name is:  David Harboyan\n"
                           "My CWID is:  887059103\n";

  // write to the pipe
  write( writeEndFileDescriptor, write_msg, strlen(write_msg)); /* intentionally does not write the null character at the end */
  printf("parent wrote:   \n%s\n", write_msg);




  /****************************************************************************
  ** Completely unnecessary and unrelated for this program to work, but just to provide an example of allocating and releasing dynamic memory in C
  ****************************************************************************/
  char * buffer = malloc( sizeof(write_msg) );   // allocate uninitialized memory (includes space for the null character at the end of the message)

  memcpy(buffer, write_msg, sizeof(write_msg));  // do something with the memory allocated, for example fill it with data

  free( buffer );                                // release the memory (avoid memory leaks)
  buffer = NULL;                                 // defensive programming suggestion (makes more sense if the function had more lines of code that followed)
}
