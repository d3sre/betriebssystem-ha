/* 
 * File:   main.c
 * Author: des
 *
 * Created on September 28, 2014, 9:23 PM
 */

#include <stdio.h>
#include <unistd.h>
int num = 0;
int main(int argc, char*argv[]){
    int pid;
    pid = fork();
    printf("pid of fork(): ", pid);
    printf("num: %d\n", num);  
    if(pid == 0){       /*child*/
        num = 1;
    }else if(pid > 0){  /*parent*/
        num = 2;
        execl("/bin/ls", "/bin/ls", "-r", "-t", "-l", (char *) 0);
    }
    printf("%d\n", num);
}
                