// TAR RELATED FUNCTIONS

/*
*   Creates a tar file from specified input directory (in_dir)
*   and outputs the tar file to output directory (out_dir)
*/
void create_tar_file(char *in_dir, char *out_dir)
{
    char in[MAXPATHLEN];
    strcpy(in, in_dir);
    char out[MAXPATHLEN];
    strcpy(out, out_dir);

    //fork process so child process can complete function
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) //Child process
    {
        printf("Creating tar file from %s, output: %s\n", in, out);
        char *args[] = {"tar", "-C", in, "-cf", out, ".", NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }
    else //Parent process waits for child and then continues
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/*
*   Expands a tar file from specified input tar file (in_dir)
*   and outputs the contents to output directory (out_dir)
*/
void expand_tar_file(char *in_tar_file, char *out_dir)
{
    char in[MAXPATHLEN];
    strcpy(in, in_tar_file);
    char out[MAXPATHLEN];
    strcpy(out, out_dir);
    pid_t pid = fork(); //fork process so child process can complete function
    if (pid == -1)
    {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) //Child process
    {
        printf("Expanding tar file %s to %s\n", in, out);
        char *args[] = {"tar", "-C", out, "-xf", in, NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }
    else //Parent process waits for child and then continues
    {
        int status;
        waitpid(pid, &status, 0);
    }
}