/* 
 * File:   main.c
 * Author: des
 *
 * Created on October 19, 2014, 9:51 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    
    int area(struct shape *shape)
    {
        return shape->area(shape);
    }

    int circumference(struct shape *shape)
    {
        return shape->circumference(shape);
    };

    void output(struct shape *shape)
    {
        return shape->output(shape);
    }
    
    int circle_area(struct shape *shape) {
        int r = shape->a;
        return M_PI*r*r;
    }

    int circle_circumference(struct shape *shape) {
        int r = shape->a;
        return 2*M_PI*r;
    }

    void circle_output(struct shape *shape) {
        printf("Circle r = %\n" , shape->a);
    }

    void circle_init(struct shape *shape, int r) {
        //create pointers to correct functions ("quasi inheritance")
        shape->area=circle_area;
        shape->circumference=circle_circumference;
        shape->output = circle_output;
        // initialize variables
        shape->a = r;
        shape->b = 0;
        shape->c = 0;
    }

    void circle_init_random(struct shape *shape) {
        int r = randomInteger();
        circle_init(shape, r);
    }

    
    int main(int argc, char *argv[]) {
        printf("Define shape where circumference and area shall be calculated with random values: \n");
        printf("0 for circle, 1 for square, 2 for triangle\n");
        int input;
        scanf("%d", &input);
        
        //size_t cnt = (size_t) (argc -1);
        
        struct shape *figure1 = (struct shape *) malloc(1 * sizeof(struct shape));
        
        switch(input) {
            case '0' :
                circle_init_random(figure1);
                break;
            default : 
                printf("argument invalid");
                break;
        }
        
    }
  
}


