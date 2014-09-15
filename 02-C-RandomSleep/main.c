/* 
 * File:   main.c
 * Author: des
 *
 * Created on September 15, 2014, 7:36 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*
 * 
 */

void timestamp()
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    printf("%s",asctime( localtime(&ltime) ) );
}

int main(int argc, char** argv) {
    
    int counter = 0;
    int randomValue = 0;
    
    while(counter < 100) {
        randomValue = rand() % 1000000;
        timestamp();
        printf("Number: %d\n", counter);
        usleep(randomValue);
        counter = counter + 1;
        
    }

    return (EXIT_SUCCESS);
}

