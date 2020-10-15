int cleanup() {
    //deletes all temporary folders and files, and clears memory.
    printf("cleaning files\n");
    return 0;
}

void remove_directory(char* d) {
    char dir[MAXPATHLEN];
    int i = 0;
    while(d[i] != '\0') {
        dir[i] = d[i];
        i++;
    }
    
    pid_t pid = fork();
    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        //printf("child process running command\n");
        printf("Removing directory: %s\n", dir);
        char *args[] = {"rm", "-rf", dir,  NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    } else {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}