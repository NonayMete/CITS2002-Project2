char *progname;

#define TEMPLATE "/tmp/mt-XXXXXX"
//creates and outputs temp directory with specified TEMPLATE
char *create_temp_directory() {
    char newdirname[] = TEMPLATE;
    mkdtemp(newdirname);
    printf("Created temp directory: %s\n", newdirname);
    char *name = newdirname;
    return name;
}

//moves file or directory to somewhere else
void move_file(char *in_file, char *out_file) {
    char *infile = malloc(MAXPATHLEN);
    strcpy(infile, in_file);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_file);

    pid_t pid = fork();
    if(pid == -1) {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        printf("Moving file %s to %s\n", infile, outfile);
        char *args[] = {"mv", "-v", infile, outfile,  NULL};
        //need to check 'z'
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    } else {
        int status;
        //printf("waiting for child process\n");
        waitpid(pid, &status, 0);
    }
}



void file_attributes(char *filename) {
    struct stat stat_buffer;

    if(stat(filename, &stat_buffer) != 0) { // can we 'stat' the file's attributes?
        perror( progname );
        exit(EXIT_FAILURE);
    }
    else if( S_ISREG( stat_buffer.st_mode ) ) {
        printf( "%s is a regular file\n", filename );
        printf( "is %i bytes long\n", (int)stat_buffer.st_size );
        printf( "and was last modified on %i\n", (int)stat_buffer.st_mtime);

        printf( "which was %s", ctime( &stat_buffer.st_mtime) );
    }
}

// void list_directory(char *dirname)
// {
//     DIR *dirp;
//     struct dirent *dp;

//     dirp = opendir(dirname);
//     if(dirp == NULL) {
//         perror( progname );
//         exit(EXIT_FAILURE);
//     }
//     printf("testes\n");
//     while((dp = readdir(dirp)) != NULL) {  
//         printf( "%s\n", dp->d_name );
//     }
//     closedir(dirp);
// }

//compares two files and outputs which one should take priority
char *compare_files(char *file1_, char *file2_) {
    
}

void merge_directories(char *dir_name, char *out_file)
{
    char *dirname = malloc(MAXPATHLEN);
    strcpy(dirname, dir_name);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_file);
    printf("Reading directory: %s\n", dirname);
    char fullpath[MAXPATHLEN];
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(dirname);
    if(dirp == NULL) {
        perror( progname );
        exit(EXIT_FAILURE);
    }

    while((dp = readdir(dirp)) != NULL) {
        struct stat stat_buffer;
        sprintf(fullpath, "%s/%s", dirname, dp->d_name);
        if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        if(stat(fullpath, &stat_buffer) != 0) {
             perror( progname );
        }
        else if( S_ISDIR( stat_buffer.st_mode )) {
            printf( "%s is a directory\n", fullpath );
        }
        else if( S_ISREG( stat_buffer.st_mode )) {
            printf( "%s is a regular file\n", fullpath );
        }
        else {
            printf( "%s is unknown!\n", fullpath );
        }

        //check if file exists
        char checkfile[MAXPATHLEN];
        sprintf(checkfile, "%s/%s", outfile, dp->d_name);
        printf("checkfile: %s\n", checkfile);
        if(access(checkfile, F_OK) != 0) {
            //if not move the file/directory there
             move_file(fullpath, outfile);
        } else { 
            //if it does exist then preform checks to see which one wins
            printf("%s already exists not moving\n", checkfile);
            compare_files();
        }



    }
    closedir(dirp);
}