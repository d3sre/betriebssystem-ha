/*
 * File:   newcunittest.c
 * Author: des
 *
 * Created on Sep 22, 2014, 2:59:52 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "main.h"

/*
 * CUnit Test Suite
 */

/**/
int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void delete(node**);

void testDelete() {
    node** p2;
    delete(p0);
    if (1 /*check result*/) {
        CU_ASSERT(0);
    }
}

void generate(node**, int);

void testGenerate() {
    node** p2;
    int p3;
    generate(p0, p1);
    if (1 /*check result*/) {
        CU_ASSERT(0);
    }
}

void search(node*, int, int);

void testSearch() {
    node* p2;
    int p3;
    int p4;
    search(p0, p1, p2);
    if (1 /*check result*/) {
        CU_ASSERT(0);
    }
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("newcunittest", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "testDelete", testDelete)) ||
            (NULL == CU_add_test(pSuite, "testGenerate", testGenerate)) ||
            (NULL == CU_add_test(pSuite, "testSearch", testSearch))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
