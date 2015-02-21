/* 
 * File:   main.c
 * Author: des
 *
 * Created on February 21, 2015, 5:38 PM
 */

#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char** argv) {

        int k,m,n;
        int fdout;
        //int fdin;
        const char *FILENAME = "loesung.txt";
        //const char *CONTENT = "Dies ist ein Beispieltext 2";
        //char *buffer;
        
        //öffnet file FILENAME für schreiben (WRONLY), file wird erstellt falls es noch nicht gibt (CREAT)
        //fdout enthaelt file descriptor wert fuer neu erstelles file
        //plus file permission hinzugefuegt
        fdout = open(FILENAME, O_WRONLY | O_CREAT, 0777);
        //ausgabe des soeben erhaltenen file descriptor wertes
        printf("fdout before=%d\n", fdout);
        
        //static int square(int num);
        int i;
        int result;
        int tempfdout;
        
        int square(int num){
            return num*num;
        }
        
        char str[6];
        
        for(i=100; i>0; i--) {
            sprintf(str, "%d", i);
            write(fdout, (void *) str, strlen(str));
            write(fdout, (void *) "\n", 1);
//            printf("fdout before=%d\n", fdout);
//            result = square(i);
//            int l=!result;
//            int value = result;
//                while (value) { 
//                    l++; 
//                    value/=10; 
//                }
//            //printf("i=%d (length=%d)\n", i,l);
//            //get length    
//            n = l + 1; 
//            
//            tempfdout = fdout; // save old fdout
//            fdout = fdout - n;
//            //schreibe ab ort fdout, content i, laenge n
//            m = write(fdout, (void *) result, n);
//            
//            printf("%d bytes written (length %d) => %d \n", m, n, result);
//            printf("fdout after=%d\n", fdout);
        }
        

        
        close(fdout);
        
        //buffer = (char *) malloc(m);
        //fdin = open(FILENAME, O_RDONLY);
        //printf("fdin=%d\n", fdin);
        //k = read(fdin, (void *) buffer, m);
        //close(fdin);
        //printf("%d byte read:\n%s\n", k, buffer);
        exit(0);
}

