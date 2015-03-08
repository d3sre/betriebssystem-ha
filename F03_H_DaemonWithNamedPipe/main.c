/* 
 * File:   main.c
 * Author: des
 *
 * Created on March 2, 2015, 9:11 PM
 * Source from 
 * https://github.com/bk1/sysprogramming-examples/blob/master/posix/processes/daemonize-with-pipe.c
 * 
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

//#include <itskylib.h>

int main(int argc, char *argv[]) {
  int fork_result;
  //int pipes[2];
  char buff[1024];
  int retcode;
  int fdout;
  const char *FILENAME = argv[1]; 
  FILE *file;   
  /* message content is not relevant... */
  char message[] = "4";
  printf("Started parent\n");
  //1
  retcode = mkfifo("namedPipe", 0666);
  //retcode = pipe(pipes);
  //handle_error(retcode, "Error creating pipe", PROCESS_EXIT);
  
  printf("generating numbers\n");
  retcode = open("namedPipe", O_WRONLY);
  write(retcode, buff, strlen(message));
  
  
  /* first fork() to create child */
  fork_result = fork();
  if (fork_result == 0) {
    printf("In child\n");
    

    /* second fork to create grand child */
    fork_result = fork();
    if (fork_result != 0) {
      /* exit child, make grand child a daemon */
      printf("exiting child\n");
      exit(0);
    }
    
    /* in grand child */
    /* close write end of pipe */
    //retcode = close(pipes[1]);
    // open mit fd erzeugen
    //open("namedPipe", O_RDONLY);
    //handle_error(retcode, "error closing pipe in grandchild (daemon)", PROCESS_EXIT);

    /* read from pipe */
    printf("in grandchild: waiting for pipe\n");
    //open("namedPipe", O_RDONLY);
    
    retcode = open("namedPipe", O_RDONLY);
    
    retcode = read(retcode, buff, strlen(message));
    //handle_error(retcode, "error reading from pipe in grandchild", PROCESS_EXIT);
    
    //***
    //fdout = open(FILENAME, O_WRONLY);
    //write(STDOUT_FILENO, message, fdout );
    int i = (int*)buff;
    int square = i*i;
    fopen(FILENAME, "a");
    fprintf(file, "Number: %d, Square: %d", i, square );
            
    //**        
    printf("grandchild has received message from grand parent: daemonized\n");
    /* pipe has done its purpose, close it */
    retcode = close(retcode);
    //handle_error(retcode, "error closing pipe in daemon", PROCESS_EXIT);

    printf("daemon has ppid=%d\n", getppid());

    /* do daemon stuff */
    sleep(30);
    printf("done with daemon\n");
    exit(0);
  }

  /* in parent */
  //close(pipes[0]);
  int status;
  printf("parent waiting for child\n");
  wait(&status);
  printf("child terminated\n");
  retcode = open("namedPipe", O_WRONLY);
  //retcode = write(pipes[1], message, strlen(message));
  //handle_error(retcode, "error writing to pipe in parent", PROCESS_EXIT);

  //retcode = close(pipes[1]);
  //handle_error(retcode, "error closing pipe in parent", PROCESS_EXIT);
  
  printf("parent done\n");
  exit(0);
}

