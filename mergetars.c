#include "mergetars.h"

#define TEMPLATE "/tmp/mt-XXXXXX"
#define TEMPLATE_LENGTH 14

char *create_temp_directory() {
    char newdirname[] = TEMPLATE;
    mkdtemp(newdirname);
    printf("Created temp directory: %s\n", newdirname);
    char *name = newdirname;
    return name;
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
        exit(EXIT_FAILURE);
    }
    for(int i=0; i < argc; i++) {
        if(strlen(argv[i]) >= MAXPATHLEN) {
            printf("Error path name too long.");
            exit(EXIT_FAILURE);
        }
    }
    char *output_tar_file = malloc(MAXPATHLEN);
    strcpy(output_tar_file, argv[argc-1]);

    //create a temporary file to merge everything into
    char *temp_out_file = malloc(MAXPATHLEN);
    strcpy(temp_out_file, create_temp_directory());

    //loop through all 
    for(int i = 1; i < argc-1; i++) {
        char *input_file = malloc(MAXPATHLEN);
        strcpy(input_file, argv[i]);
        //printf("input_file: %s\n", input_file);
        char *temp_directory = malloc(MAXPATHLEN);
        strcpy(temp_directory, create_temp_directory());

        //printf("temp directory: %s\n", temp_directory);
        expand_tar_file(input_file, temp_directory);
        list_directory(temp_directory, temp_out_file);
        remove_directory(temp_directory);
        printf("\n");
    }

    create_tar_file(temp_out_file, output_tar_file);
    remove_directory(temp_out_file);
    //list_directory("/tmp/");
    // create_tar_file(input_file, output_tarfile);
    // remove_directory(output_tarfile);
    cleanup();
    return 0;
}


