/* 
 * File:   main.c
 * Author: des
 *
 * Created on February 16, 2015, 10:02 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

/*http://stackoverflow.com/questions/3554120/open-directory-using-c
 * http://man7.org/linux/man-pages/man3/opendir.3.html
 * http://stackoverflow.com/questions/779910/should-i-use-char-argv-or-char-argv-in-c
 * http://stackoverflow.com/questions/10186765/char-array-vs-char-pointer-in-c
 * 
 */
int main(int argc, char** argv) {
   
    DIR *pDir;
    
    
    pDir = opendir(const char argv);
    

    return (EXIT_SUCCESS);
}

