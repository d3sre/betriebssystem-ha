#include <iostream>
#include <stdio.h>   /* required for file operations */


using namespace std;



int main(int argc, char *argv[]) {

    //get name of file as argument from program start
    char *name = "";
    if (argc == 2) {
        name = argv[1];
    }

    if (argc > 2) {
        printf("Usage\n\n");
        printf("%s -c\ncleanup ipc\n\n", argv[0]);
        printf("%s -s\nsetup ipc\n\n", argv[0]);
        printf("%s < inputfile\ncout file, show accumulated output\n\n", argv[0]);
        printf("%s name < inputfile\ncout file, show output with name\n\n", argv[0]);
        exit(1);
    }

    while (TRUE) {
        ssize_t size_read = read(STDIN_FILENO, buffer, BUF_SIZE);
        if (size_read == 0) {
/* end of file */
            break;
        }
    }

}


/***
* Steps:
* 1: read file
* 2: sort by characters and printout
* 3: use 2 threads with barriers to access the same information
* 4: use bubble sort at first (or qsort library -> for sorting arrays of strings, integers and structs)
*/
