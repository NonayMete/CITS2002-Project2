void remove_directory(char *d)
{
    char dir[MAXPATHLEN];
    strcpy(dir, d);
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        //printf("child process running command\n");
        printf("Removing directory: %s\n", dir);
        char *args[] = {"rm", "-rf", dir, NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}