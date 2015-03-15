/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 * 
 * Source started with https://github.com/bk1/sysprogramming-examples/blob/master/posix/ipc/daemonize-group-signal.c
 * 
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0
//#include <itskylib.h>


int daemonized = FALSE;
void signal_handler1(int signo) 
{
    daemonized = TRUE;
    int pid = getpid();
    int pgid = getpgid(pid);
    int ppid = getppid();
    printf("signal %d received pid=%d pgid=%d ppid=%d\n", signo, pid, pgid, ppid);
}
void signal_handler2(int signo) 
{
    daemonized = TRUE;
    int pid = getpid();
    int pgid = getpgid(pid);
    int ppid = getppid();
    printf("signal %d received pid=%d pgid=%d ppid=%d\n", signo, pid, pgid, ppid);
    printf("%d: deamon is ready\n", pid);
}

int main(int argc, char *argv[]) 
{
int retcode;

  int fork_result;
  int status;
  int pid = getpid();
  int pgid = pid;

  /* set pgid to pid */
  retcode = setpgid(pid, pgid);
  //handle_error(retcode, "setpgid", PROCESS_EXIT);

  /* set signal handler */
  signal(SIGUSR1, signal_handler1);
  signal(SIGUSR2, signal_handler2);
  // void *ptr = signal(SIGUSR1, signal_handler);
  // handle_ptr_error(ptr, "signal", PROCESS_EXIT);
  /* first fork() to create child */
  fork_result = fork();
  //handle_error(fork_result, "fork (1)", PROCESS_EXIT);
  if (fork_result == 0) {
    printf("%d: In child\n", getpid());

    /* second fork to create grand child */
    fork_result = fork();
  //  handle_error(fork_result, "fork (2)", PROCESS_EXIT);
    if (fork_result != 0) {
      /* exit child, make grand child a daemon */
      printf("%d: exiting child\n", getpid());
      exit(0);
    }
    
    /* in daemon (grand child) */
    
    pid = getpid();
    while (! daemonized) {
      pause();
    }
    
    printf("%d: daemon has pgid=%d ppid=%d\n", pid, getpgid(pid), getppid());
    kill(getpgid(pid), SIGUSR2);

    /* do daemon stuff */
    sleep(30);
    printf("%d: done with daemon\n", getpid());
    exit(0);
  }
  
  printf("%d: parent waiting for child\n", getpid());
  retcode = wait(&status);
  //handle_error(retcode, "wait", PROCESS_EXIT);
  printf("%d: child terminated status=%d\n",getpid(), status);
  printf("%d: kill -pgid: %d\n",getpid(), -pgid);

  retcode = kill(-pgid, SIGUSR1);
 // handle_error(retcode, "kill", PROCESS_EXIT);
 
  pause();
  
  printf("%d: parent done\n", getpid());
  exit(0);
}