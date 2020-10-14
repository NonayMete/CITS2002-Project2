#include "mergetars.h"

int main(int argc, char *argv[])
{
    //Check command line arguments
    if (argc < 3) {
        printf("Incorrect arguments given\n");
        //exit(EXIT_FAILURE);
    }
    printf("hello World!\n");
    int pid = fork();
    printf("PID: %i", getpid());
    
    char *args[2] = {"tar -cvf testfromcode.tar Project1", NULL};
    execvp("tar", args);
    cleanup();
    return 0;
}
