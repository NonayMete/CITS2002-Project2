#include "mergetars.h"

int main(int argc, char *argv[])
{
    //Check command line arguments
    if (argc < 3)
    {
        printf("Incorrect arguments: too few arguments given\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < argc; i++)
    {
        if (strlen(argv[i]) >= MAXPATHLEN)
        {
            printf("Error path name too long.");
            exit(EXIT_FAILURE);
        }
    }
    char *output_tar_file = malloc(MAXPATHLEN);
    strcpy(output_tar_file, argv[argc - 1]);

    //create a temporary file to merge everything into
    char *temp_out_file = malloc(MAXPATHLEN);
    strcpy(temp_out_file, create_temp_directory());

    //loop through all input tar files
    for (int i = 1; i < argc - 1; i++)
    {
        char *input_file = malloc(MAXPATHLEN);
        strcpy(input_file, argv[i]);
        
        //create a temp directory to expand tar file into 
        char *temp_directory = malloc(MAXPATHLEN);
        strcpy(temp_directory, create_temp_directory());

        //expand the tar file and merge it into output file
        expand_tar_file(input_file, temp_directory);
        merge_directories(temp_directory, temp_out_file, "");

        //remove temporary directory
        remove_directory(temp_directory);
        printf("\n");
    }

    //create output tar file from temperory output dir
    create_tar_file(temp_out_file, output_tar_file);
    //delete last temporary directory
    remove_directory(temp_out_file);
    return 0;
}
