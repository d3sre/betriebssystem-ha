/* 
 * File:   main.c
 * Author: des
 *
 * Created on March 9, 2015, 8:31 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/*
 * source from http://stackoverflow.com/questions/13477714/how-to-send-signal-sigusr1-and-sigusr2-from-parent-to-children
 * 
 */
int n;

sigset_t sig_m1, sig_m2, sig_null;
int signal_flag=0;

void start_signalset();

void sig_func(int signr) {
  printf("%d\n", signr, n);

  start_signalset();
}

void start_signalset() {
  if(signal(SIGUSR2, sig_func) == SIG_ERR) {
    exit(0);
  }

 if(signal(SIGUSR1, sig_func) == SIG_ERR) {
    exit(0);
  }
}

void wait_for_parents() {
    while(signal_flag == 0) {
        sigsuspend(&sig_null);
    }
}


int main(){
    int result,pt_pid;
    start_signalset();
    pt_pid=getpid(); 

    result = fork();
    if(result==-1){ 

        printf("Can't fork child\n");
        exit(-1);
    } else if (result == 0) {

            wait_for_parents();

    } else {
       kill(result,SIGUSR2);
       kill(result,SIGUSR2);
       kill(result,SIGUSR1);
       kill(result,SIGUSR2);

        signal_flag = 1;
    }    
    return 0; 
}


