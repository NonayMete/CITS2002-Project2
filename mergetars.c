#include "mergetars.h"

int main(int argc, char *argv[])
{
    //Check command line arguments
    if (argc < 3) {
        printf("Incorrect arguments given\n");
        //exit(EXIT_FAILURE);
    }
    printf("hello World! 1\n");
    //int pid = fork();
    //printf("PID: %i", getpid());
    
    char *args[] = {"tar", "-cvf", "testfromcode.tar", "Project1",  NULL};
    execvp(args[0], args);
    cleanup();
    return 0;
}
