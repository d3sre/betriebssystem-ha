/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*
* Aufgabenstellung:
* Gleichzeitiger Zugriff auf shared memory von mehreren prozessen,
* über semaphore verwaltet.
*
*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define PERM 0600
#define BUF_SIZE 16384
#define ALPHA_SIZE 256
#define SEM_SIZE 128
#define SEM_LIMIT  10

#define TRUE 1
#define FALSE 0
#define PROCESS_EXIT -1
#define NO_EXIT -1

const char *REF_FILE = "./shm_sem_ref.dat";

void handle_error(int code, const char *text, int flags) {
    if(code == -1) {
        printf("Error Code: %d, %s\n", code, text);
        exit(flags);
    }
}

int shmid_for_cleanup = 0;
int semid_for_cleanup = 0;
int semid_out_for_cleanup = 0;

struct data {
    long counter[ALPHA_SIZE];
};

const int SIZE = sizeof(struct data);

//semun (fourth argument of semctl) according to http://linux.die.net/man/2/semctl
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */ // data structure defined in sys_sem.h
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

// get an XSI shared memory segment, returns shared memory identifier associated with key
int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
    int shm_id = shmget(key, SIZE, flags | PERM);
    handle_error(shm_id, etxt, PROCESS_EXIT);
    return shm_id;
}
// gets semaphore set identifier
int create_sem(key_t key, const int sem_size, const char *txt, const char *etxt, int flags) {
    int semaphore_id = semget(key, sem_size, flags | PERM);
    handle_error(semaphore_id, etxt, PROCESS_EXIT);
    return semaphore_id;
}

//setup semaphore with variable size
int setup_sem(int semaphore_id, char *etxt) {
    short semval[SEM_SIZE];
    int i;
    for (i = 0; i < SEM_SIZE; i++) {
        semval[i] = (short) SEM_LIMIT;
    }
    int retcode = semctl(semaphore_id, SEM_SIZE, SETALL, &semval);
    handle_error(retcode, etxt, PROCESS_EXIT);
    return retcode;
}


int setup_sem_out(int semaphore_out_id, char *etxt) {
    short semval[1];
    semval[0] = (short) 1;
    int retcode = semctl(semaphore_out_id, 1, SETALL, &semval);
    handle_error(retcode, etxt, PROCESS_EXIT);
    return retcode;
}

void cleanup() {
    if (shmid_for_cleanup > 0) {
        int retcode = shmctl(shmid_for_cleanup, IPC_RMID, NULL);
        shmid_for_cleanup = 0;
        handle_error(retcode, "removing of shm failed", NO_EXIT);
    }
    if (semid_for_cleanup > 0) {
        int retcode = semctl(semid_for_cleanup, 0, IPC_RMID, NULL);
        semid_for_cleanup = 0;
        handle_error(retcode, "removing of semaphore failed", NO_EXIT);
    }
    if (semid_out_for_cleanup > 0) { //TODO remove
        int retcode = semctl(semid_out_for_cleanup, 0, IPC_RMID, NULL);
        semid_out_for_cleanup = 0;
        handle_error(retcode, "removing of semaphore failed", NO_EXIT);
    }
}


void show_shm_ctl(int shm_id, const char *txt) {

    int retcode;
    struct shmid_ds shmctl_data;
    retcode = shmctl(shm_id, IPC_STAT, &shmctl_data);
    handle_error(retcode, "child shmctl failed", PROCESS_EXIT);
    struct ipc_perm perms = shmctl_data.shm_perm;
    printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
}

void show_sem_ctl(int semaphore_id, int idx, const char *txt) {

    int retcode;
    struct semid_ds semctl_data;
    // struct semun    semctl_extra;
    retcode = semctl(semaphore_id, idx, IPC_STAT, &semctl_data);
    handle_error(retcode, "child semctl failed", PROCESS_EXIT);
    int val = semctl(semaphore_id, idx, GETVAL, &semctl_data);
    handle_error(retcode, "child semctl failed", PROCESS_EXIT);
    struct ipc_perm perms = semctl_data.sem_perm;
    printf("%6d %s: t=%ld key=%ld idx=%d val=%d nsems=%d uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", (int) getpid(), txt, (long) time(NULL), (long) perms.__key, idx, (int) val, (int) semctl_data.sem_nsems, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
    fflush(stdout);
}

int main(int argc, char *argv[]) {

    time_t t_start = time(NULL);

    if (argc > 2) {
        printf("Usage\n\n");
        printf("%s -c\ncleanup ipc\n\n", argv[0]);
        printf("%s -s\nsetup ipc\n\n", argv[0]);
        printf("%s < inputfile\ncout file, show accumulated output\n\n", argv[0]);
        printf("%s name < inputfile\ncout file, show output with name\n\n", argv[0]);
        exit(1);
    }

    int retcode = 0;

    // equivalent to open(path, O_WRONLY|O_CREAT|O_TRUNC, mode)
    // create a new file or rewrite an existing one
    int fd = creat(REF_FILE, S_IRUSR | S_IWUSR);

    // create IPC key (from file (path) and project ID)
    key_t shm_key = ftok(REF_FILE, 1);
    if (shm_key < 0) {
        handle_error(-1, "ftok failed", PROCESS_EXIT);
    }

    //create second  IPC key for same file
    key_t sem_key = ftok(REF_FILE, 2);
    if (sem_key < 0) {
        handle_error(-1, "ftok failed", PROCESS_EXIT);
    }
    //second semaphore IPC key
    key_t sem_key_out = ftok(REF_FILE, 3);
    if (sem_key_out < 0) {
        handle_error(-1, "ftok failed", PROCESS_EXIT);
    }
    //creationg with 2 semaphore
    if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        printf("setting up IPC\n");
        //get shared memory segment for IPC key of file
        int shm_id = create_shm(shm_key, "create", "shmget failed", IPC_CREAT);
        shmid_for_cleanup = shm_id;
        //gets semaphore for IPC key of file
        int semaphore_id = create_sem(sem_key, SEM_SIZE, "create", "semget (data) failed", IPC_CREAT);
        semid_for_cleanup = semaphore_id;
        //gets 2nd semaphore access for IPC key of file
        int semaphore_out_id = create_sem(sem_key_out, 1, "create", "semget (out) failed", IPC_CREAT);
        semid_for_cleanup = semaphore_id;

        show_sem_ctl(semaphore_id, 0, "semaphore before setup");
        setup_sem(semaphore_id, "semaphore setup failed");
        show_sem_ctl(semaphore_id, 0, "semaphore after setup");
        show_sem_ctl(semaphore_out_id, 0, "semaphore out before setup");
        setup_sem_out(semaphore_out_id, "semaphore out setup failed");
        show_sem_ctl(semaphore_out_id, 0, "semaphore out after setup");
        printf("done\n");
        exit(0);
    }


    int shm_id = create_shm(shm_key, "create", "shmget failed", 0);
    shmid_for_cleanup = shm_id;
    int semaphore_id = create_sem(sem_key, SEM_SIZE, "create", "semget failed", 0);
    semid_for_cleanup = semaphore_id;
    int semaphore_out_id = create_sem(sem_key_out, 1, "create out", "semget out failed", 0);
    semid_out_for_cleanup = semaphore_out_id;

    if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        printf("cleaning up IPC\n");
        cleanup();
        exit(0);
    }

    char *name = "";
    if (argc == 2) {
        name = argv[1];
    }

    struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);

    time_t total_data_semops_wait = 0;
    char buffer[BUF_SIZE];
    long *counter = shm_data->counter;


    while (TRUE) {
        //read from file (and get the amount read)
        ssize_t size_read = read(STDIN_FILENO, buffer, BUF_SIZE);
        if (size_read == 0) {
            /* end of file */
            break;
        }
        handle_error(size_read, "error while reading stdin", PROCESS_EXIT);
        int i;
        for (i = 0; i < size_read; i++) {
            //does what program has to do (can vary depending of task definition)
            //this case read the i character and count that character
            unsigned char c = buffer[i];
//            unsigned int  ck = (c % SEM_SIZE);
//            struct sembuf semops_write;
//            semops_write.sem_num = ck;
//            semops_write.sem_op  = -SEM_LIMIT;
//            semops_write.sem_flg = SEM_UNDO;
//            time_t t0 = time(NULL);
//            show_sem_ctl(semaphore_id, ck, "reserving write semaphore");
//            retcode = semop(semaphore_id, &semops_write, 1);
//            handle_error(retcode, "error while getting write-semaphore", PROCESS_EXIT);
//            show_sem_ctl(semaphore_id, ck, "write semaphore reserved");
//            time_t dt = time(NULL) - t0;
//            total_data_semops_wait += dt;
            counter[c]++;
//            semops_write.sem_num = ck;
//            semops_write.sem_op  = SEM_LIMIT;
//            semops_write.sem_flg = SEM_UNDO;
//            show_sem_ctl(semaphore_id, ck, "freeing write semaphore");
//            retcode = semop(semaphore_id, &semops_write, 1);
//            handle_error(retcode, "error while releasing write-semaphore", PROCESS_EXIT);
//            show_sem_ctl(semaphore_id, ck, "write semaphore freed");
        }
    }
    unsigned int i;

    struct sembuf semops_output;
    semops_output.sem_num = 0;
    semops_output.sem_op  = -1;
    semops_output.sem_flg = SEM_UNDO;
    time_t t1 = time(NULL);
    show_sem_ctl(semaphore_out_id, 0, "reserving output semaphore");
    //semaphore oeprations again,
    retcode = semop(semaphore_out_id, &semops_output, 1);
    handle_error(retcode, "error while getting output-semaphore", PROCESS_EXIT);
    time_t dt = time(NULL) - t1;
    show_sem_ctl(semaphore_out_id, 0, "output semaphore reserved");
    time_t total_duration = time(NULL) - t_start;

    printf("------------------------------------------------------------\n");
    printf("%s: pid=%ld\n", name, (long) getpid());
    printf("total wait for data: ~ %ld sec; total wait for output: ~ %ld sec total duration=%ld\n", (long) total_data_semops_wait, (long) dt, (long) total_duration);
    printf("------------------------------------------------------------\n");
    for (i = 0; i < ALPHA_SIZE; i++) {
        long *counter = shm_data->counter;

//        struct sembuf semops_read;
//        unsigned int  ck = (i % SEM_SIZE);
//        semops_read.sem_num = ck;
//        semops_read.sem_op  = -1;
//        semops_read.sem_flg = SEM_UNDO;
//        show_sem_ctl(semaphore_id, ck, "reserving read semaphore");
//        retcode = semop(semaphore_id, &semops_read, 1);
//        handle_error(retcode, "error while getting read-semaphore", PROCESS_EXIT);
//        show_sem_ctl(semaphore_id, ck, "read semaphore reserved");

        long val = counter[i];

//        semops_read.sem_num = ck;
//        semops_read.sem_op  = 1;
//        semops_read.sem_flg = SEM_UNDO;
//        show_sem_ctl(semaphore_id, ck, "freeing read semaphore");
//        retcode = semop(semaphore_id, &semops_read, 1);
//        handle_error(retcode, "error while releasing read-semaphore", PROCESS_EXIT);
//        show_sem_ctl(semaphore_id, ck, "read semaphore freed");

        if (! (i & 007)) {
            printf("\n");
            fflush(stdout);
        }
        if ((i & 0177) < 32 || i == 127) {
            printf("\\%03o: %10ld    ", i, val);
        } else {
            printf("%4c: %10ld    ", (char) i, val);
        }
    }
    printf("\n\n");
    printf("------------------------------------------------------------\n\n");
    fflush(stdout);

    semops_output.sem_num = 0;
    semops_output.sem_op  = 1;
    semops_output.sem_flg = SEM_UNDO;
    show_sem_ctl(semaphore_out_id, 0, "freeing output semaphore");
    retcode = semop(semaphore_out_id, &semops_output, 1);
    handle_error(retcode, "error while releasing output-semaphore", PROCESS_EXIT);
    show_sem_ctl(semaphore_out_id, 0, "output semaphore freed");

    retcode = shmdt(shm_data);
    handle_error(retcode, "error while detaching shared memory", PROCESS_EXIT);
    cleanup();
    exit(0);
}
