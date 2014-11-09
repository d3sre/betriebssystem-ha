/* 
 * File:   main.c
 * Author: des
 *
 * Created on October 19, 2014, 9:51 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "structureFunctions.h"

/*
 * 
 */
int main(void) {
    
    struct shape;
    
    typedef int (*function)(struct shape *shape);
    typedef void (*process)(struct shape *shape);
    
    
    
    struct area ball = { CIRCLE, "ball" };
    struct area warnung = { TRIANGLE, "warnung" };
    
    printf("%s says %s\n", ball.name, area_form(&ball));
    printf("%s says %s\n", warnung.name, area_form($warnung));
    

    return 0;
}

