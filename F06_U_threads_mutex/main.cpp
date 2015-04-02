/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
*/
/* use mutex for IPC, but do preparation separately */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define ERROR_SIZE 4096

// readable code snippets
#define TRUE 1
#define FALSE 0
//#define PROCESS_EXIT -1
#define NO_EXIT -1


enum exit_type { PROCESS_EXIT, THREAD_EXIT, NO_EXIT };

void handle_error(int code, const char *text, int flags) {
    if(code == -1) {
        printf("Error Code: %d, %s\n", code, text);
        exit(flags);
    }
}
void exit_by_type(enum exit_type et) {
    switch (et) {
        case PROCESS_EXIT:
            exit(1);
            break;
        case THREAD_EXIT:
            pthread_exit(NULL);
            break;
        case NO_EXIT:
            printf("continuing\n");
            break;
        default:
            printf("unknown exit_type=%d\n", et);
            exit(2);
            break;
    }
}


void handle_thread_error(int retcode, const char *msg, enum exit_type et) {
   if (retcode == -1) {
   printf("Error Code: %d, %s\n", retcode, msg);
       int flags = (int)et;
   exit(flags);
   }
}
}
// end of readable code snippet


pthread_mutex_t mutex;
//struct timespec from time.h, contains of tv_sec and tv_nsec
struct timespec timeout;
int use_timeout;
void *run(void *arg) {
    int retcode;
    printf("in child: sleeping\n");
    sleep(2);
    if (use_timeout) {
        retcode = pthread_mutex_timedlock(&mutex, &timeout);
    } else {
        retcode = pthread_mutex_lock(&mutex);
    }
    handle_thread_error(retcode, "child failed (timed)lock", PROCESS_EXIT);
    printf("child got mutex\n");
    sleep(5);
    printf("child releases mutex\n");
    pthread_mutex_unlock(&mutex);
    printf("child released mutex\n");
    sleep(10);
    printf("child exiting\n");
    return NULL;
}
void usage(char *argv0, char *msg) {
    printf("%s\n\n", msg);
    printf("Usage:\n\n%s\n lock mutexes without lock\n\n%s -t <number>\n lock mutexes with timout after given number of seconds\n", argv0, argv0);
    exit(1);
}
int main(int argc, char *argv[]) {
    int retcode;
    use_timeout = (argc >= 2 && strcmp(argv[1], "-t") == 0);
    if (is_help_requested(argc, argv)) {
        usage(argv[0], "");
    }
    //timeout from struct timespec
    timeout.tv_sec = (time_t) 200;
    timeout.tv_nsec = (long) 0;
    if (use_timeout && argc >= 3) {
        int t;
        sscanf(argv[2], "%d", &t);
        timeout.tv_sec = (time_t) t;
    }
    printf("timout(%ld sec %ld msec)\n", (long) timeout.tv_sec, (long) timeout.tv_nsec);
    //create thread (phtread_t thread) and start thread with phtread_create, without remember return value
    pthread_t thread; pthread_create(&thread, NULL, run, NULL);
    printf("in parent: setting up\n");
    //mutex/thread initialize
    pthread_mutex_init(&mutex, NULL);
    sleep(2);
    printf("in parent: getting mutex\n");
    if (use_timeout) {
        //create mutex with timeout (when use_timeout flag is set)
        retcode = pthread_mutex_timedlock(&mutex, &timeout);
    } else {
        //create mutex lock on file
        retcode = pthread_mutex_lock(&mutex);
    }

    handle_thread_error(retcode, "parent failed (timed)lock", PROCESS_EXIT);
    printf("parent got mutex\n");
    sleep(5);
    printf("parent releases mutex\n");
    //free section
    pthread_mutex_unlock(&mutex);
    printf("parent released mutex\n");
    printf("parent waiting for child to terminate\n");
    //collect threads
    pthread_join(thread, NULL);
    //free mutex completely
    pthread_mutex_destroy(&mutex);
    printf("done\n");
    exit(0);
