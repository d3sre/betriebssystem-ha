/* 
 * File:   main.c
 * Author: des
 *
 * Created on February 21, 2015, 5:38 PM
 */

/*
 * Vorgehensweise
 * 1. Definition von variablen
 * 2. Schlaufe von 100 bis 1
 * 3. Ausgabe jeweils Quadratzahl von Wert
 * 4- Position muss jeweils vorher gewählt werden (mit lseek suchen)
 * 5. Quadrat ausrechnen ev in Subfunktion
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int square(int num){
    return num*num;
}

int getNumberLength(int num) {
    char str[6];
    sprintf(str, "%d", num); // write as ascii (string), not binary
    int l = strlen(str);

    //n = l + 1; 
    return l;
}


int main() {

        int n;
        int fdout;

        const char *FILENAME = "loesung.txt";
        
        //opens file FILENAME for write only (WRONLY), file will be created if not existing yet (CREAT)
        //fdout has file descriptor value for newly created file
        //plus file permission added
        fdout = open(FILENAME, O_WRONLY | O_CREAT, 0777);
        //printout file descriptor value
        //printf("fdout before=%d\n", fdout);
        
        int i;
        int result;
        int totallength = 0;
        int previouslength = 0;
        int stepsback;
        char str[6];
        int filePosition;
        int bytesWritten;

        //get total length of file
        for(i=100;i>0;i--) {
            
            result = square(i);        
            totallength = totallength + getNumberLength(result) + 1;
        }
        
        //start output to file
        for(i=100; i>0; i--) {

            result = square(i);     
            n = getNumberLength(result) + 1;
            stepsback = previouslength + n;
            
            switch(i)
            {
              // case 1: lseek(fdout, -n, SEEK_CUR);
                case 100:
                    lseek(fdout, totallength-n, SEEK_SET);
                    break;
                default : 
                    lseek(fdout, -stepsback, SEEK_CUR);
                    break;
            }
              
            //output written to file + bytesWritten variable for check
            sprintf(str, "%d\n", result); // write as ascii (string), not binary
            bytesWritten = write(fdout, (void *) str, strlen(str));
            
            // caching value for calculation fo correct amount of backsteps
            previouslength = n;

            //testoutput for checks
            filePosition = lseek (fdout, 0, SEEK_CUR);
 //           printf("i: %d \t\t  square: %d \t\t n: %d \t\t stepsback: %d \t\t fileposition: %d \t\t BytesWritten: %d \t\t Stringlength: %d\n", i, result, n, stepsback, filePosition, bytesWritten, strlen(str));
            
        }
        printf("total length=%d\n", totallength);
     
        close(fdout);

        exit(0);
}

