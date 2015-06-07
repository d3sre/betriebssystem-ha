/* (C) IT Sky Consulting GmbH / Steve Heller 2014
 * http://www.it-sky-consulting.com/
 * Author: Steve Heller
 * Date: 2014-06-02
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>


#include <itskylib.h>

enum blocking { BLOCKING, NON_BLOCKING };

int setup = 0;

void do_blocking() {
    while(TRUE) {
        char input_string[256];
        char *p;
        unsigned int inputStringLen;
        //read from command line
        char *is = fgets(input_string, sizeof(input_string), stdin);
        handle_ptr_error(is, "fgets", PROCESS_EXIT);
        //locates \n in string and if it occures, replaces it with \0
        if ((p = strchr(input_string, '\n')) != NULL) {
            *p = '\0';
        }
        printf("You entered: %s\n",input_string);
    }
}

void do_non_blocking() {

    //changes file descriptor to nonblocking
    int retcode = fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    handle_error(retcode, "fctnl F_SETFL O_NONBLOCK", PROCESS_EXIT);
    while(TRUE) {
        char input_string[256];
        char *p;
        unsigned int inputStringLen;
        //fill input_string with 0
        memset(input_string, 0, sizeof(input_string));
        //read command line
        char *is = fgets(input_string, sizeof(input_string), stdin);
        //EAGAIN is error code for nonzero receive timeout
        if (is == NULL && errno != EAGAIN) {
            handle_ptr_error(is, "fgets", PROCESS_EXIT);
        }
        //locates \n in strings and if it occures, replaces it with \0
        if ((p = strchr(input_string, '\n')) != NULL) {
            *p = '\0';
        }
        if (input_string[0] == (char) 0) {
            printf("You entered nothing!\n");
        } else {
            printf("You entered: %s\n", input_string);
        }
        sleep(1);
    }
}

void do_stuff() {
    //check enum state of variable setup (defined at startup)
    if (setup == BLOCKING) { // blocking
        do_blocking();
    } else if (setup == NON_BLOCKING) { // non-blocking
        do_non_blocking();
    } else {
        printf("WRONG USAGE, TRY AGAIN\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage:\n%s -b for blocking\n%s -n for non-blocking\n",argv[0],argv[0]);
        exit(1);
    }
    // if mode -b was defined at startup it's blocking
    if (strcmp(argv[1], "-b") == 0) {
        setup = BLOCKING;
        //if mode -n was defined at startup it's non blocking
    } else if (strcmp(argv[1], "-n") == 0) {
        setup = NON_BLOCKING;
    }
    do_stuff();
    exit(0);
}