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

#include <fcntl.h>     // O_CREAT, O_RDWR
#include <stdio.h>     // printf(), stderr
#include <unistd.h>    // ftruncate(), fork(), execvp()

#include <sys/mman.h>  // shm_open(), mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, shm_unlink()
#include <sys/shm.h>
#include <sys/time.h>  // timeval, gettimeofday(), timersub()
#include <sys/wait.h>  // wait()

/*******************************************************************************
** This implementation uses shared memory.  The child process gets the start
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
  // Shared memory setup
  // 1) create the shared memory region
  char sharedMemoryName[] = "Command Timer";
  int  sharedMemoryFD     = shm_open( "shm", O_CREAT | O_RDWR, 0666 );

  // 2) configure the size of the shared memory region
  ftruncate( sharedMemoryFD, sizeof(timeval_t) );

  // 3) memory map the shared memory region
  timeval_t * sharedMemory = mmap( 0, sizeof(timeval_t), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFD, 0 );



  /*****************************************************************************
  ** Create the subprocess to execute the timed command, letting the child
  ** record the start time
  *****************************************************************************/
  if( fork() == 0 ) // child process
  {
    // Get and save the current time into shared memory,
    gettimeofday( sharedMemory, 0 );

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
    timeval_t startTime;
    startTime = *sharedMemory;    // The child placed the start time in shared memory
    shm_unlink( "shm" );

    // calculate elapsed time
    timeval_t elapsed_time;
    timersub( &end_time, &startTime, &elapsed_time );


    // print microseconds right justified zero filled
    printf( "\nElapsed time: %ld.%06ld seconds\n", elapsed_time.tv_sec, elapsed_time.tv_usec );
    printf( "IPC Method: Shared Memory\nCommand: " );
    char ** arg = argv + 1;
    while( *arg ) printf("%s ", *arg++);
    printf("\n");
  }

  return 0;
}
