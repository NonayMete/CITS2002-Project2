#include "mergetars.h"

#define TEMPLATE "/tmp/mt-XXXXXX"
#define TEMPLATE_LENGTH 14
//MAXPATHLEN

char *create_temp_directory() {
    char newdirname[] = TEMPLATE;
    mkdtemp(newdirname);
    printf("Created temp directory: %s\n", newdirname);
    char *name = newdirname;
    return name;
}

void create_tar_file(char* in_file, char* out_file) {
    char in[MAXPATHLEN];
    int i = 0;
    while(in_file[i] != '\0') {
        in[i] = in_file[i];
        i++;
    }
    char out[MAXPATHLEN];
    i = 0;
    while(out_file[i] != '\0') {
        out[i] = out_file[i];
        i++;
    }

    pid_t pid = fork();
    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        printf("Creating tar file from %s, output: %s\n", in, out);
        char *args[] = {"tar", "-cvf", out, in,  NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    } else {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}


    //expand tar files into temp directories
    //find and store info about all files inside tars
    //create new temp file and merge everything
    //turn new file into tar
    //delete all temp files.

int main(int argc, char *argv[])
{
    //Check command line arguments
    if (argc < 3) {
        printf("Incorrect arguments: too few arguments given\n");
        //exit(EXIT_FAILURE);
    }

    char *input_file = "Project1/";
    char *output_tarfile = argv[argc-1];
    create_tar_file(input_file, output_tarfile);
    remove_directory(output_tarfile);


    char *temp_directory = create_temp_directory();
    int temp_directory_length = (int) strlen(temp_directory);
    printf("why: %i\n", temp_directory_length);
    remove_directory(temp_directory);    

    cleanup();
    return 0;
}


