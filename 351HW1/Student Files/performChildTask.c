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

#include <stdio.h>        // printf(), fprintf(), scanf(), fscanf(), stderr, stdin, stdout
#include <unistd.h>       // pipe(), close(), fork(), write(), read()

#include "performChildTask.h"


/******************************************************************************
** PRIVATE TYPES and DATA
******************************************************************************/
// none


/******************************************************************************
** Function definitions
******************************************************************************/
void performChildTask( const int readEndFileDescriptor )
{
  char character;
  printf("\nchild read:     \n");

  // read and print one character at a time from the pipe while not end of file
  while( read( readEndFileDescriptor, &character, 1 ) > 0)    printf("%c", character);
}
