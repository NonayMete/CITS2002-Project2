#include "mergetars.h"

int main(int argc, char *argv[])
{
    //Check command line arguments
    if (argc < 3) {
        printf("Incorrect arguments given\n");
        //exit(EXIT_FAILURE);
    }
    printf("hello World! 1\n");

    pid_t pid = fork();

    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        printf("child process running command\n");
        char *args[] = {"tar", "-cvf", "testfromcode.tar", "Project1",  NULL};
        execvp(args[0], args);
    } else {
        int status;
        printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }

    cleanup();
    return 0;
}
