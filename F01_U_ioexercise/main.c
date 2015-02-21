/* 
 * File:   main.c
 * Author: des
 *
 * Created on February 16, 2015, 8:32 PM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {

    int n; // length of text
    
    //const char *FILE = "/home/des/os-uebung-160216.txt";
    const char *CONTENT = "Beispiel1";
    char *buffer;
    /*
     * char s[6]
     * for (..)
     * sprintf(s, "%4d \n" i*i);
     * write (fd <-> s <-> 5)
     
     */
    int k, m, n;
    int fdout;
    int fdin;
    const char *FILENAME = "./out-lowlever.txt";
    char line[6];
    n = 5;
    fdout = open(FILENAME, O_WRONLY | O_CREAT);
    
    n =strlen(CONTENT) + 1;
    for(i=0; i<100; i++) {
        
    }
    
    return (EXIT_SUCCESS);
}

