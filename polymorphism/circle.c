#include "structureFunctions.h"
#include <math.h>
#include "main.c"

int circle_area(struct shape *shape) {
    int r = shape->a;
    return M_PI*r*r;
}

int circle_circumference(struct shape *shape) {
    int r = shape->a;
    return 2*M_PI*r;
}

void circle_output(struct shape *shape) {
    print("Circle r = " . shape->a);
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


