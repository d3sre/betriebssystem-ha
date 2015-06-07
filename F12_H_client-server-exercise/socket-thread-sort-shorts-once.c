/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* enable qsort_r */
#define _GNU_SOURCE

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include <transmission-protocols.h>
#include <hsort.h>
#include <fsort.h>
#include <isort.h>
#include <ternary-hsort.h>
#include <msort.h>
#include <fsort-metrics.h>
#include <psort.h>
#include "socket-thread-sort-shorts-once.h"

const int LINE_SIZE = 200;
const int MAXPENDING = 5;    /* Maximum outstanding connection requests */

pthread_once_t once = PTHREAD_ONCE_INIT;

pthread_key_t *create_key();

pthread_key_t *key;

//count shorts
int thread_count;
int data_size;
short *data;

enum mt_sort_type { MT_HEAP_SORT, MT_TERNARY_HEAP_SORT, MT_QUICK_SORT, MT_FLASH_SORT, MT_INSERTION_SORT, MT_MERGE_SORT };

struct thread_arg {
//    const char *file_name;
//    enum mt_sort_type selected_sort_type;
//    struct string_array **content;
//    int client_socket;
//    int thread_idx;

    int thread_idx;
    pthread_barrier_t *barrier;
    short *data;
    size_t size;
    struct thread_arg *arr;
};

void *thread_run(void *ptr);
void prepare(const char * file_name);

void transmit_sorted_words(struct thread_arg *arg);

void thread_once();


void print_data() {
    sleep(1);
    printf("------------------------------------------------------------\n");
    int w = 0;
    for (int i = 0; i < data_size; i++) {
        int x = data[i];
        printf("%6d", x);
        w+=6;
        if (w > 168) {
            printf("\n");
            w = 0;
        }
    }
    printf("\n");
    printf("------------------------------------------------------------\n");
}

void usage(char *argv0, char *msg) {
    printf("%s\n\n", msg);
    printf("Usage:\n\n");

    printf("splits the given input file into bytes and sorts them using n threads\n");
    printf("\n");
    printf("%s n file\n", argv0);
    exit(1);
}

int main(int argc, char *argv[]) {
    int retcode;
    pthread_t *thread;

    if (is_help_requested(argc, argv)) {
        usage(argv[0], "");
    }

    //key = create_key();

    char *argv0 = argv[0];
    if (argc != 3) {
        printf("found %d arguments\n", argc - 1);
        usage(argv0, "wrong number of arguments");
    }

    thread_count = atoi(argv[1]);
    const char *file_name = argv[2];

    prepare(file_name);
//    int opt_idx = 2;
//    enum mt_sort_type selected_sort_type;

//    char *argv_opt = argv[opt_idx];
//    if (strlen(argv_opt) != 2 || argv_opt[0] != '-') {
//        usage(argv0, "wrong option");
//    }
//    char opt_char = argv_opt[1];
//    switch (opt_char) {
//        case 'h' :
//            selected_sort_type = MT_HEAP_SORT;
//            break;
//        case 't' :
//            selected_sort_type = MT_TERNARY_HEAP_SORT;
//            break;
//        case 'f' :
//            selected_sort_type = MT_FLASH_SORT;
//            break;
//        case 'q' :
//            selected_sort_type = MT_QUICK_SORT;
//            break;
//        case 'i' :
//            selected_sort_type = MT_INSERTION_SORT;
//            break;
//        case 'm' :
//            selected_sort_type = MT_MERGE_SORT;
//            break;
//        default:
//            usage(argv0, "wrong option");
//            break;
//    }
//
//    char *file_name = argv[3];
//    int server_port = atoi(argv[1]);

    int server_port = 32111;
    prepare(file_name);
    /* Create socket for incoming connections */
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    handle_error(server_socket, "socket() failed", PROCESS_EXIT);
    printf("server_socket=%d server_port=%d\n", server_socket, server_port);

    struct sockaddr_in  server_address;
    /* Construct local address structure */
    memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
    server_address.sin_family = AF_INET;                /* Internet address family */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_address.sin_port = htons(server_port);      /* Local port */

    /* Bind to the local address */
    retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    handle_error(retcode, "bind() failed", PROCESS_EXIT);

    /* Mark the socket so it will listen for incoming connections */
    retcode = listen(server_socket, MAXPENDING);
    handle_error(retcode, "listen() failed", PROCESS_EXIT);

    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    int idx = 0;

    struct string_array **content = (struct string_array **)malloc(sizeof(struct string_array *));

    while (TRUE) { /* Run forever */
        struct sockaddr_in client_address; /* Client address */
        /* Set the size of the in-out parameter */
        unsigned int client_address_len = sizeof(client_address);

        /* Wait for a client to connect */
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
        handle_error(client_socket, "accept() failed", PROCESS_EXIT);

        /* client_socket is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));

        pthread_t thread;
        struct thread_arg *thread_data = (struct thread_arg *) malloc(sizeof(struct thread_arg));

        pthread_barrier_t barrier;
        retcode = pthread_barrier_init(&barrier, NULL, thread_count + 1);


        retcode = pthread_create(&thread, &thread_attr, thread_run, thread_data);
        handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
    }
    /* never going to happen: */
    exit(0);
}

void *thread_run(void *ptr) {
    int retcode;
    struct thread_arg *arg = (struct thread_arg *) ptr;

    qsort(arg->data, arg->size, 1, compare);
    int idx = arg->thread_idx;
    struct thread_arg *arr = arg->arr;
    for (int step = 1; step < thread_count; step *= 2) {
        retcode = pthread_barrier_wait(arg->barrier);
        if (retcode != PTHREAD_BARRIER_SERIAL_THREAD && retcode != 0) {
            printf("waited for barrier in child %d (step=%d retcode=%d)\n", idx, step, retcode);
            handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
        }

        if (idx % (2*step) == 0) {
            int other_idx = idx + step;
            if (other_idx < thread_count) {
                int m = arr[idx].size;
                int n = arr[other_idx].size;
                int i = 0;
                int j = 0;
                int k = 0;
                int total_len = m + n;
                char *copy = malloc(total_len);
                char *left = arr[idx].data;
                char *right = arr[other_idx].data;
                while (i + j < total_len) {
                    if (i >= m) {
                        memcpy(copy + k, right + j, n - j);
                        j = n;
                    } else if (j >= n) {
                        memcpy(copy + k, left + i, m - i);
                        i = m;
                    } else {
                        if (left[i] <= right[j]) {
                            copy[k++] = left[i++];
                        } else {
                            copy[k++] = right[j++];
                        }
                    }
                }
                arr[idx].size = total_len;
                memcpy(left, copy, total_len);
                arr[other_idx].size = 0;
                arr[other_idx].data = NULL;
                free(copy);
            }
        }
    }


    retcode = pthread_setspecific(*key, arg);
    handle_thread_error(retcode, "pthread_setspecific", THREAD_EXIT);
    transmit_sorted_words(arg);
    free(ptr);
    return (void *) NULL;
}

pthread_key_t *create_key() {
    pthread_key_t *key = (pthread_key_t *) malloc(sizeof(pthread_key_t));
    pthread_key_create(key, NULL);
    return key;
}

void threadManager() {
    size_t size_per_thread = data_size / thread_count;
    size_t rest = data_size;
    short *partial_data = data;

    for (int i = 0; i < thread_count; i++) {


        thread_data[i].thread_idx = i;
        thread_data[i].barrier = &barrier;
        thread_data[i].data = partial_data;
        thread_data[i].size = i == thread_count - 1 ? rest : size_per_thread;
        thread_data[i].arr = thread_data;

        rest -= size_per_thread;
        partial_data += size_per_thread;
        retcode = pthread_create(&(thread[i]), NULL, thread_run, &(thread_data[i]));
        handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
    }
    for (int step = 1; step < thread_count; step *= 2) {
        // printf("waiting for barrier in main\n");
        retcode =   pthread_barrier_wait(&barrier);
        // printf("waited for barrier in main (step=%d)\n", step);
        if (retcode != PTHREAD_BARRIER_SERIAL_THREAD && retcode != 0) {
            printf("waited for barrier in main (step=%d)\n", step);
            handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
        }
        // printf("sorting of partitions finished for step=%d\n", step);
    }
    for (int i = 0; i < thread_count; i++) {
        /* printf("main: joining thread %d\n", i); */
        retcode = pthread_join(thread[i], NULL);
        handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
        /* printf("main: joined thread %d\n", i); */
    }
    print_data();
    pthread_barrier_destroy(&barrier);
}

void prepare(const char * file_name) {
    // int retcode;
    printf("opening %s\n", file_name);
    int fd = open(file_name, O_RDONLY);
    handle_error(fd, "open", PROCESS_EXIT);
    struct stat stat;
    int retcode = fstat(fd, &stat);
    handle_error(retcode, "open", PROCESS_EXIT);
    off_t size = stat.st_size;
    if (size <= 0) {
        handle_error(-1, "size not positive", PROCESS_EXIT);
        exit(1);
    }
    char *buffer = (char *) malloc(size);
    retcode = read(fd, buffer, size);
    handle_error(retcode, "read", PROCESS_EXIT);
    retcode = close(fd);
    handle_error(retcode, "close", PROCESS_EXIT);
    data = buffer;
    data_size = size;
}

int compare(const void *x_ptr, const void *y_ptr) {
    char x = *((char *) x_ptr);
    char y = *((char *) y_ptr);
    return x-y;
}

void thread_once() {

    printf("in thread_once()\n");
    struct thread_arg *arg = (struct thread_arg *) pthread_getspecific(*key);

    // int retcode;
    const char *file_name = arg->file_name;
    int fd = open(file_name, O_RDONLY);
    handle_error(fd, "open", PROCESS_EXIT);
    struct string_array *content = malloc(sizeof(struct string_array *));
    *content = read_to_array(fd);
    close(fd);

    char **strings = content->strings;
    size_t len = content->len;
    switch (arg->selected_sort_type) {
        case MT_HEAP_SORT:
            hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
            break;
        case MT_TERNARY_HEAP_SORT:
            ternary_hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
            break;
        case MT_QUICK_SORT:
            qsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
            break;
        case MT_FLASH_SORT:
            fsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, metric_str_full, (void *) NULL);
            break;
        case MT_INSERTION_SORT:
            isort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
            break;
        case MT_MERGE_SORT:
            msort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
            break;
        default:
            /* should *never* happen: */
            handle_error_myerrno(-1, -1, "wrong mt_sort_type", PROCESS_EXIT);
    }
    *(arg->content) = content;

}

void transmit_sorted_words(struct thread_arg *arg) {

    int retcode;
    retcode = pthread_once(&once, thread_once);
    handle_thread_error(retcode, "pthread_once", THREAD_EXIT);

    struct string_array content = **(arg->content);
    char **strings = content.strings;
    int idx = arg->thread_idx;
    int client_socket = arg->client_socket;

    char buff[5];
    read_4byte_string(client_socket, buff);
    if (strcmp(buff, "GETX") != 0) {
        printf("received wrong message %s\n", buff);
        retcode = close(arg->client_socket);
        handle_error(retcode, "close()", THREAD_EXIT);
        return;
    }

    //FILE *stream = fdopen(arg->client_socket,"rw");
    //handle_ptr_error(stream, "fdopen(arg->client_socket,\"rw\")", THREAD_EXIT);

    // printf("fdopen done stream=%p\n", stream);

    char line[LINE_SIZE+1];

    write_string(client_socket, "\n------------------------------------------------------------\n", -1);
    sprintf(line, "thread %d\n", idx);
    write_string(client_socket, line, -1);
    write_string(client_socket, "------------------------------------------------------------\n", -1);

    char *pos = line;
    size_t used_len = 0;
    for (int i = 0; i < content.len; i++) {
        int len = strlen(strings[i]) + 1; /* allow for space before... */
        if (used_len + len > LINE_SIZE) {
            *pos = '\n';
            used_len++;
            write_string(client_socket, line, used_len);
            pos = line;
            used_len = 0;
        }
        if (len > LINE_SIZE+1) {
            write_string(client_socket, strings[i], len - 1);
            write_string(client_socket, "\n", 1);
        } else {
            sprintf(pos, " %s", strings[i]);
            pos += len;
            used_len += len;
        }
    }
    if (used_len > 0) {
        *pos = '\n';
        used_len++;
        write_string(client_socket, line, used_len);
    }

    write_string(client_socket, "------------------------------------------------------------\n\n", -1);
    write_string(client_socket, "", 0);
    printf("output transmitted\n");
    //retcode = fflush(stream);
    //handle_error(retcode, "fflush()", THREAD_EXIT);
    //retcode = fclose(stream);
    //handle_error(retcode, "fclose()", THREAD_EXIT);
    read_4byte_string(client_socket, buff);
    if (strcmp(buff, "DONE") != 0) {
        printf("received wrong message %s\n", buff);
    }

    retcode = close(arg->client_socket);
    handle_error(retcode, "close()", THREAD_EXIT);
}


