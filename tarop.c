
void create_tar_file(char* in_file, char* out_file) {
    char in[MAXPATHLEN];
    strcpy(in, in_file);
    char out[MAXPATHLEN];
    strcpy(out, out_file);
    pid_t pid = fork();
    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        printf("Creating tar file from %s, output: %s\n", in, out);
        char *args[] = {"tar", "-C", in,"-cf", out, ".",  NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    } else {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}

void expand_tar_file(char *in_tar_file, char *output_file) {
    char in[MAXPATHLEN];
    strcpy(in, in_tar_file);
    char out[MAXPATHLEN];
    strcpy(out, output_file);
    pid_t pid = fork();
    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        printf("Expanding tar file %s to %s\n", in, out);
        char *args[] = {"tar", "-C", out, "-xf", in,  NULL};
        //need to check 'z'
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    } else {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}