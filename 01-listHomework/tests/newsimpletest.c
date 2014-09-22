/*
 * File:   newsimpletest.c
 * Author: des
 *
 * Created on Sep 22, 2014, 3:32:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

/*
 * Simple C Test Suite
 */

void generate(node**, int);

void testGenerate() {
    node** p0;
    int p1;
    generate(p0, p1);
    if (1 /*check result*/) {
        printf("%%TEST_FAILED%% time=0 testname=testGenerate (newsimpletest) message=error message sample\n");
    }
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% newsimpletest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%%  testGenerate (newsimpletest)\n");
    testGenerate();
    printf("%%TEST_FINISHED%% time=0 testGenerate (newsimpletest)\n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}
