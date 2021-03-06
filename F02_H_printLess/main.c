/* 
 * File:   main.c
 * Author: des
 *
 * Created on March 1, 2015, 2:27 PM
 */

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

/*
 * Definition:
 * 1 Zeile = eine ganze Laenge Ausgabe im Konsolenfenster (variable cols)
 * **
 * 
 * 1. get size console window (for buffer size)
 * 2. read as many characters as fit into window
 * 3. print out all read characters
 * 4. wait for input
 * 
 */
int fdout;


/*
 * read console dimension - source: 
 * http://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/
 * 
 */
void getConsoleWindowSize(int* pcols, int* plines) {
    #ifdef TIOCGSIZE
        struct ttysize ts;
        ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
        *pcols = ts.ts_cols;
        *plines = ts.ts_lines;
    #elif defined(TIOCGWINSZ)
        struct winsize ts;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
        *pcols = ts.ws_col;
        *plines = ts.ws_row;
    #endif /* TIOCGSIZE */
}

/*
 * read line by receiving the fd value, current position to read, the pointer to the buffer and 
 * the size that should be read
 */
int readLine(int fdout, int pos, char* pOutBuffer, int size) {    
    int charsRead = 0;
    int lineSize = 0;
    int i;
    
    lseek(fdout, pos, SEEK_SET);
    charsRead = read(fdout, pOutBuffer, size);
    
    /*
     * check if the next line read contains a "\n" and if yes, remember the location and 
     * give this information back
     */
    for (i = 0; i < charsRead; i++)
    {
        if (pOutBuffer[i] == '\n')
        {
            lineSize = i+1;
            return lineSize;
        }
    }
    
    lineSize = charsRead;
    return lineSize;
}


/*
 * Source from: 
 * http://ubuntuforums.org/showthread.php?t=225713&page=2&s=9360ceae8150887a6cf1cb3497926c8c
 * reads 1 character from input without displaying it
 */
int getch(void)
{
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
    newt = oldt; /* copy old settings to new settings */
    newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
    ch = getchar(); /* standard getchar call */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
    return ch; /*return received char */
}
/*
 * Main Function, started with 1 argument (the file that should be displayed)
 */
int main(int argc, char** argv) {
    
    int cols = 80;
    int lines = 24;
    char* pLine = NULL;
    int charsRead = 0;
    char key;
    int pos;
    _Bool quit;
    _Bool validInput;
    
    /*
     * error when no argument delivered or too many are given
     */
    if(argv[2] != NULL || argv[1] == NULL) {
        printf("Program requires 1 argument (filename) \n");
        return -1;
    }
    else {
        getConsoleWindowSize(&cols, &lines);
    
        /* allocate needed space for one line
         */
        pLine = malloc(cols + 1);
    
        /* work with file name that is argv[1]
         */
        const char *FILENAME = argv[1]; 
    
        fdout = open(FILENAME, O_RDONLY, 0777);
        pos = 0;
        quit = 0;
        validInput = 1;
        
        /*
         * run program while not quitted by user
         */
        while (quit == 0)
        {
            /* 
             * only read next line if valid input was received
             */
            if (validInput)
            {
                /* return the line size 
                 */
                charsRead = readLine(fdout, pos, pLine, cols);
        
                /* insert new line character at the end of the line
                 */
                pLine[charsRead] = '\0';  

                /* print line
                 */
                printf("%s", pLine);
                //TO-DO: print complete window size - 
                /* - for loop for print out  as many lines there are
                 * - fill each charsRead value into an array to remember when jumping back
                 *    challenge: defining array size for these charsRead as only complete when full file is read
                 * 
                 */
            }
            
            /* read character for input (see method getch())
             */
            key = getch();
            /*
             * menu that selects action for input-key
             */
            switch (key)
            {
                case 'k':
                    pos = pos + charsRead;
                    validInput = 1;
                    break;
                case 'q':
                    quit = 1;
                    validInput = 1;
                    break;
                //TO-DO: implement more cases like j, f, g  
                    /*
                     * j: previous line: print out full window but from line before
                     * f: jump to beginning pos=0
                     * g: jump to end (needs to be calculated first)
                     * other options like one page down uses window size, rest from calculation point of view is 
                     * same as line up or down
                     */
                default:
                    validInput = 0;
                    break;
            }
        }
    
        free(pLine);
    }
    return (EXIT_SUCCESS);
}

