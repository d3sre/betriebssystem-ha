/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
* Basic: https://github.com/bk1/sysprogramming-examples/blob/master/posix/threads/thread-memory-count-by-file.c
*/

/****
* sachedes - 05.04.2015
* The file contains two versions, one counting characters and one counting shorts. The counting characters part is commented out,
* the current version displays the counted shorts as the exercise was specified as "counts the frequencies of occurrences of values"
* where the values are interpreted as the shorts.
* Like also commented in the code further below, this way we won't have an interpreted ascii value displayed as this would require a NUL before
* a low other value which can't be read during a file. Therefore the output may look weird.
* Also as of always reading 2 bytes, it's possible that the file contains an invalid length of data, in this case this information is echoed
* and the last read short sequence is discarded.
*
* The probably more reasonable way of counting characters is still available, it references the old struct data and the ALPHA_SIZE, where
* as the counting shorts references struct data2 and the BETA_SIZE.
*
* The code was compiled with 'gcc -lpthread main.c'
*
*/
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>



void handle_error(int code, const char *text, int flags) {
    if(code == -1) {
        printf("Error Code: %d, %s\n", code, text);
        exit(flags);
    }
}

#define TRUE 1
#define FALSE 0
#define PROCESS_EXIT -1
#define NO_EXIT -1
#define THREAD_EXIT -1

#define BUF_SIZE 16384
#define ALPHA_SIZE 256
#define BETA_SIZE 65536
#define SEM_SIZE 128

// ***Counting characters
//struct data {
//    long counter[ALPHA_SIZE];
//};
// ***Counting shorts
struct data2 {
    long counter[BETA_SIZE];
};
// ***Counting characters
//const int SIZE = sizeof(struct data);
// ***Counting shorts
const int SIZE2 = sizeof(struct data2);
//define thread mutex variable
static pthread_mutex_t output_mutex;
// ***Counting characters
//static struct data data;
// ***Counting shorts
static struct data2 data2;

/* thread function */
void *run(void *raw_name) {
    // int retcode;
    time_t thread_start = time(NULL);
    char *name = (char *) raw_name;
    int fd = -1;
    //time when thread was started
    time_t open_start = time(NULL);

    //int fd, initialized to -1 at beginning of thread
    while (fd == -1) {
        // open file received when thread was opened in read only mode, return value is file descriptor
        // fd = 0 standard input
        // fd = 1 standard output
        // fd = 2 standard error
        fd = open(raw_name, O_RDONLY);
        // output received and no error there -> sleep and go forward
        if (fd < 0 && errno == EMFILE) {
            sleep(1);
            continue;
        }
        //printf("opened file %s \n",raw_name);
        //output received print error message
        if (fd < 0) {
            char msg[256];
            sprintf(msg, "error while opening file=%s", name);
            handle_error(fd, msg, THREAD_EXIT);
        }
    }
    // measure time how long reading file took
    time_t open_duration = time(NULL) - open_start;
    // set a time measuring variable for amount of time mutex needs to wait to 0
    time_t total_mutex_wait = 0;
    //char buffer[BUF_SIZE];
    short shortValue;
    // ***Counting characters
    //struct data local_data;
    //long *scounter = local_data.counter;
    // ***Counting shorts
    struct data2 local_data2;
    long *scounter2 = local_data2.counter;
    //read file until done
    while (TRUE) {
        //read file at file descriptor as long as it is into buffer
        ssize_t size_read = read(fd, &shortValue, sizeof(shortValue));
        if (size_read == 0) {
            /* end of file */
            break;
        }
        //error message when file couldn't be read
        if (size_read < 0) {
            char msg[256];
            sprintf(msg, "error while reading file=%s", name);
            handle_error(size_read, msg, THREAD_EXIT);
        }
        // ***Counting characters
        //int i;
        //count every occurance of a character in ascii
        //for (i = 0; i < size_read; i++) {
        //    unsigned char c = *(((unsigned char*)(&shortValue)) + i);
        //printf("short: %d, %d. unsigned char: %d\n", shortValue,i+1, c);
        //    scounter[c]++;
        //}
        // ***Counting shorts
        if (size_read == 1){
            printf("file contains a remaining byte, will be discarded");
        }
        else {
            //count every occurance of a character in short(unicode)
            scounter2[shortValue]++;
        }

    }
    //close file reading at filedescriptor
    close(fd);
    //calculate needed amount of time
    time_t thread_duration = time(NULL) - thread_start;
    unsigned int i;
    // ***Counting characters
    //long *tcounter = data.counter;
    // ***Counting shorts
    long *tcounter2 = data2.counter;
    //lock section for printout of all counters of one file
    pthread_mutex_lock(&output_mutex);

    printf("------------------------------------------------------------\n");
    printf("%s: pid=%ld\n", name, (long) getpid());
    printf("open duration: ~ %ld sec; total wait for data: ~ %ld sec; thread duration: ~ %ld\n", (long) open_duration, (long) total_mutex_wait, (long) thread_duration);
    printf("------------------------------------------------------------\n");
    // ***Counting characters
//    for (i = 0; i < ALPHA_SIZE; i++) {
//        tcounter[i] += scounter[i];
//        long val = tcounter[i];
//        if (val != 0) {
////            if (! (i & 007)) {
////                printf("\n");
////            }
//            if ((i & 0177) < 32 || i == 127) {
//                printf("\\%03o: %20ld \n", i, val);
//            } else {
//                printf("%4c: %20ld \n", (char)i, val);
//            }
//        }
//    }
    //fgetc(stdin);
    // ***Counting shorts
    for (i = 0; i < BETA_SIZE; i++) {
        tcounter2[i] += scounter2[i];
        long val = tcounter2[i];
//        if (val != scounter2[i]) {
//         //   printf("tcounter: %d, scounter: %d\n", val, scounter2[i]);
//            fgetc(stdin);
//        }

        if (val != 0) {
//            if (! (i & 007)) {
//                printf("\n");
//            }
            if (i <= 32 || i >= 127) {
                printf("\\%06o: %20ld \n", i, val);
            } else {
                //can not be reached as 2 ascii chars would need to be in a row, where NUL would be the lead character and NUL can't be typed into a file
                printf("%7c: %20ld \n", (char)i, val);
            }
        }
    }
    printf("\n\n");
    printf("------------------------------------------------------------\n\n");
    fflush(stdout);
    //end lock and jump out of thread
    pthread_mutex_unlock(&output_mutex);
    return NULL;
}


int main(int argc, char *argv[]) {
    //checks for enough parameters
    if (argc < 2) {
        printf("Usage\n\n");
        printf("%s file1 file2 file3 ... filen\ncount files, show accumulated output after having completed one file\n\n", argv[0]);
        exit(1);
    }
    //record start time
    time_t start_time = time(NULL);
    //initialisation & getting ready
    int retcode = 0;
    int i;
    printf("%d files will be read\n", argc-1);
    fflush(stdout);
    // ***Counting characters
    //fill struct data size 256 (ALPHA_SIZE) with type long 0
//    for (i = 0; i < ALPHA_SIZE; i++) {
//        data.counter[i] = 0L;
//    }
    // ***Counting shorts
    for (i = 0; i < BETA_SIZE; i++) {
        data2.counter[i] = 0L;
    }
    //threat mutex initialize (variable was defined before, called output_mutex
    retcode = pthread_mutex_init(&output_mutex, NULL);
    handle_error(retcode, "creating mutex", PROCESS_EXIT);
    //malloc receives casted size, counts received arguments -1 multiplies with the size of a phread_t (from pthreadtypes.h)
    pthread_t *threads = malloc((argc-1) * sizeof(pthread_t)); //before
    //void *threads = malloc((argc-1) * sizeof(pthread_t)); //after

    //*threads = (pthread_t) *threads;
    //for as many arguments received (all files + 1)
    for (i = 1; i < argc; i++) {
        //create thread, return value int: int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
        // started routine is "run"
        retcode = pthread_create(&(threads[i-1]), NULL, run, argv[i]);
        //printf("opened thread returncode %d \n",retcode);
        handle_error(retcode, "starting thread", PROCESS_EXIT);
    }
    //get mutex (lock section)
    pthread_mutex_lock(&output_mutex);
    printf("%d threads started\n", argc-1);
    //Flush output buffer stream
    fflush(stdout);
    //free mutex section
    pthread_mutex_unlock(&output_mutex);
    //for as many files received
    for (i = 0; i < argc-1; i++) {
        //fetch all threads, one by one
        retcode = pthread_join(threads[i], NULL);
        handle_error(retcode, "joining thread", PROCESS_EXIT);
    }
    //free up mutex completely
    retcode = pthread_mutex_destroy(&output_mutex);
    handle_error(retcode, "destroying mutex", PROCESS_EXIT);
    //calculcate amount of time
    time_t total_time = time(NULL) - start_time;
    printf("total %ld sec\n", (long) total_time);
    printf("done\n");
    exit(0);
}