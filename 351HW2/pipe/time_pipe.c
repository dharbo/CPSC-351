/*******************************************************************************
**  This program will be run as “./time <command [args]>” and will report the
**  amount of elapsed time to run the specified command. The example output
**  below reports the amount of time to run the command ls:
**      ./time ls
**      time.c
**      time
**
**      Elapsed time: 0.25422
**
** Note:  Error handling has been intentionally omitted for now.
*******************************************************************************/

#include <fcntl.h>       // O_CREAT, O_RDWR
#include <stdio.h>       // printf(), stderr
#include <unistd.h>      // ftruncate(), fork(), execvp()

#include <sys/mman.h>    // shm_open(), mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, shm_unlink()
#include <sys/shm.h>
#include <sys/time.h>    // timeval, gettimeofday(), timersub()
#include <sys/wait.h>    // wait()

/*******************************************************************************
** This implementation uses an anonymous pipe.  The child process gets the start
** time and passes that to the parent process.  When the child terminates, the
** parent gets the end time, determines the difference between the two times,
** and outputs the elapsed time to standard output.
*******************************************************************************/


int main( int argc, char **argv )
{
  /*****************************************************************************
  ** Validate this program was launched with a command to be timed
  *****************************************************************************/
  if( argc <= 1 )
  {
    fprintf( stderr, "usage:  %s [args...]\n", argv[0] );
    return -1;
  }


  /*****************************************************************************
  ** Define a convenience alias for the type of data to be shared
  *****************************************************************************/
  typedef struct timeval timeval_t;  // type of object to share



  /*****************************************************************************
  ** IPC Setup
  *****************************************************************************/
  // Pipe setup
  enum {READ_END = 0, WRITE_END};

  // 1) define array of file descriptors
  int pipeFD[2];

  // 2) create the pipe
  pipe( pipeFD );



  /*****************************************************************************
  ** Create the subprocess to execute the timed command, letting the child
  ** record the start time
  *****************************************************************************/
  if( fork() == 0 ) // child process
  {
    // close the unused end of the pipe
    close( pipeFD[READ_END] );

    // Get and write the current time into the pipe,
    timeval_t startTime;
    gettimeofday( &startTime, 0 );
    write( pipeFD[WRITE_END], &startTime, sizeof(timeval_t) );

    // close the pipe, we're done with it
    close( pipeFD[WRITE_END] );

    // then execute the command with arguments
    execvp( argv[1], argv + 1 );
  }


  else // parent process
  {
    // Let the child do its thing, then calculate the elapsed time using the stat time from shared memory
    wait( NULL );

    // get the end time
    timeval_t end_time;
    gettimeofday( &end_time, 0 );

    // get the start time
    timeval_t  startTime;
    // close the unused end of the pipe
    close( pipeFD[WRITE_END] );

    // read starting time from the pipe
    read( pipeFD[READ_END], &startTime, sizeof(timeval_t) );

    // close the pipe, we're done with it
    close( pipeFD[READ_END] );

    // calculate elapsed time
    timeval_t elapsed_time;
    timersub( &end_time, &startTime, &elapsed_time );


    // print microseconds right justified zero filled
    printf( "\nElapsed time: %ld.%06ld seconds\n", elapsed_time.tv_sec, elapsed_time.tv_usec );
    printf( "IPC Method: Anonymous Pipe\nCommand: " );
    char ** arg = argv + 1;
    while( *arg ) printf("%s ", *arg++);
    printf("\n");
  }

  return 0;
}
