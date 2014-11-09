/* 
 * File:   main.c
 * Author: des
 *
 * Created on October 19, 2014, 9:51 PM
 */

#include <stdio.h>
#include <stdlib.h>
//#include "structureFunctions.h"

/*
 * 
 */
int main(void) {
    
    struct shape;
    
    typedef int (*function)(struct shape *shape);
    typedef void (*process)(struct shape *shape);
    
    struct shape {
        process output;
        function circumference;
        function area;
        int a;
        int b;
        int c;
    };
    
    int charToDecimal(char c) {
        return (int) (c-'0');
    }
    
    int randomInteger() {
        return (rand() % 35);
    }
    
    int main(int argc, char *argv[]) {
        printf("Define shape where circumference and area shall be calculated with random values: \n");
        printf("0 for circle, 1 for square, 2 for triangle\n");
        int input;
        scanf("%d", &input);
        
        switch(input) {
            case '0' :
                circle_init_random()
        }
        
    }
  
}

