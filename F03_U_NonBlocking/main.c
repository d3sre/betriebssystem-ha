/* 
 * File:   main.c
 * Author: des
 *
 * Created on March 2, 2015, 8:05 PM
 */

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

int fdout;

/*
 * 
 */
int main(int argc, char** argv) {
    
    //int OutBuffer = 80;
    int size = 80;
    int charsRead = 0;
    char* pLine = NULL;
    
    if(argc != 2) {
        printf("Program requires 1 argument (filename) \n");
        return -1;
    }
    else {
        pLine = malloc(size + 1);
        const char *FILENAME = argv[1]; 
        fdout = open(FILENAME, O_RDONLY|O_NONBLOCK);
        
        while (1) {
            charsRead = read(fdout, pLine, size);
            if(charsRead == 0) {
                break;
            }
            
            else    
                if(charsRead == -1) {
                    if(errno==EAGAIN) {
                       printf("\n waiting \n");
                       sleep(1);
                    }
                    else {
                        exit(1);
                    }
                }
                else{
                    write(STDOUT_FILENO, pLine, charsRead);
                    //printf("%s",pLine);
                }
            
        }
        
    }

    return (EXIT_SUCCESS);
}

