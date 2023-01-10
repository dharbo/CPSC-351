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

/************************************************************************************************************************************
** Example program to
** 1) help verify your C development environment
** 2) provide basic C coding syntax and style examples
** 3) provide basic multi-file build examples
**
**
** @author Silberschatz, Galvin, and Gagne
** Operating System Concepts  - Tenth Edition
** Figures 3.21 & 3.22
** Copyright John Wiley & Sons - 2018
**
** Modified by T. L. Bettens
** 09-JUL-2019 - Added "hello World" boilerplate, separated translation units
************************************************************************************************************************************/

#include <stdio.h>                // printf(), fprintf(), scanf(), fscanf(), stderr, stdin, stdout
#include <unistd.h>               // pipe(), close(), fork(), write(), read()
#include <sys/types.h>            // pid_t

#include "performChildTask.h"
#include "performParentTask.h"




int main( void )
{
  // 1. Create an unnamed, unidirectional pipe
  int                fileDescriptors[2] ={0, 0};
  const unsigned int READ_END           = 0;
  const unsigned int WRITE_END          = 1;

  if(pipe( fileDescriptors ) == -1)
  {
    fprintf( stderr, "Pipe failed" );
    return 1;
  }



  // 2. Now fork a child process
  pid_t pid = fork();


  /****************************************************************************
  ** PARENT PROCESS
  ** 3a. Follow the parent process
  ****************************************************************************/
  if(pid > 0)
  {
    // close the parent's unused (read) end of the pipe
    close( fileDescriptors[READ_END] );

    // have the parent perform its task, sending any output through the pipe
    performParentTask( fileDescriptors[WRITE_END] );

    // close the parent's used (write) end of the pipe
    close( fileDescriptors[WRITE_END] );
  }


  /****************************************************************************
  ** CHILD PROCESS
  ** 3b.  Follow the child process
  ****************************************************************************/
  else if(pid == 0)
  {
    /* close the unused end of the pipe */
    close( fileDescriptors[WRITE_END] );

    /* have the child perform its task, accepting any input from the pipe */
    performChildTask( fileDescriptors[READ_END] );

    /* close the write end of the pipe */
    close( fileDescriptors[READ_END] );
  }


  /****************************************************************************
  ** PROCESS CREATION FAILED
  ****************************************************************************/
  else // pid < 0
  {
    fprintf( stderr, "Fork failed" );
    return 1;
  }


  return 0;
}
