
/* Driver Program
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAX_ARGS 4
int main(void) {
    time_t t;
    char s[256];
    int i, j;
    pid_t p, pp;
//
    char* argv[MAX_ARGS];
    argv[0] = "./all_month.csv";    // the file name
    argv[1] = "10000";                // size of data
    argv[2] = "********** IN Process 1 **********";
    argv[3] = NULL;
//
    t = time(NULL);
    printf("Driver Time: %s\n", ctime(&t));
    p = getpid();
    pp = getppid();
    printf("Driver PID: %d   Driver PPID: %d \n", p, pp);
    p = fork();
    if (p == 0) {
        /* child */
        printf("child");
        execv("./main", argv);
    }
    if (p == 0) {
        printf("Driver after fork, child: %d\n", p);
    } else {
        printf("Driver after fork, parent: %d\n", p);
    }
    sleep(10);
    t = time(NULL);
    printf("Driver Time again: %s\n", ctime(&t));
    exit(0);
}
	
