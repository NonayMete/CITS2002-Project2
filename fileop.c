char *progname;

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

void merge_directories(char *dir_name, char *out_file)
{
    char *dirname = malloc(MAXPATHLEN);
    strcpy(dirname, dir_name);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_file);
    printf("Reading directory: %s\n", dirname);
    char  fullpath[MAXPATHLEN];
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(dirname);
    if(dirp == NULL) {
        perror( progname );
        exit(EXIT_FAILURE);
    }

    while((dp = readdir(dirp)) != NULL) {
        struct stat  stat_buffer;
        sprintf(fullpath, "%s/%s", dirname, dp->d_name );
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
        move_file(fullpath, outfile);

    }
    closedir(dirp);
}