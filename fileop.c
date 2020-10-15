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
    printf("testing attributes of: %s\n", filename);
    if(stat(filename, &stat_buffer) != 0) { // can we 'stat' the file's attributes?
        perror( progname );
        exit(EXIT_FAILURE);
    }
    else if(S_ISREG(stat_buffer.st_mode)) {
        printf( "%s is a regular file\n", filename );
        printf( "is %i bytes long\n", (int)stat_buffer.st_size );
        printf( "and was last modified on %i\n", (int)stat_buffer.st_mtime);

        printf( "which was %s", ctime( &stat_buffer.st_mtime) );
    }
    else if( S_ISDIR( stat_buffer.st_mode )) {
            printf( "%s is a directory\n", filename );
    }
}


//compares two files and outputs which one should take priority
char *compare_files(char *file1_, char *file2_) {
    file_attributes(file1_);
    file_attributes(file2_);
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


void merge_directories(char *dir_name, char *out_file, char *parent_file)
{
    char *dirname = malloc(MAXPATHLEN);
    strcpy(dirname, dir_name);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_file);
    // char *parentfile = malloc(MAXPATHLEN);
    // strcpy(parent_file, parent_file);
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
            char newdir[MAXPATHLEN];
            sprintf(newdir, "%s/%s%s", outfile, parent_file, dp->d_name);
            DIR *dir = opendir(newdir);
            if (dir) {
                closedir(dir);
            } else {
                //directory doesnt exist create it
                printf("creating directory %s\n", newdir);
                mkdir(newdir, 0700);
            }
            char parent[MAXPATHLEN];
            sprintf(parent, "%s%s/", parent_file, dp->d_name);
            merge_directories(fullpath, outfile, parent);
        }
        else if( S_ISREG( stat_buffer.st_mode )) {
            printf( "%s is a regular file\n", fullpath );

            //check if file exists
            char checkfile[MAXPATHLEN];
            sprintf(checkfile, "%s/%s%s", outfile, parent_file, dp->d_name);
            printf("checkfile: %s\n", checkfile);
            if(access(checkfile, F_OK) != 0) {
                //if not move the file there
                char newfile[MAXPATHLEN];
                sprintf(newfile, "%s/%s", outfile, parent_file);
                move_file(fullpath, newfile);
            } else { 
                //if it does exist then preform checks to see which one wins
                printf("%s already exists not moving\n", checkfile);
                compare_files(checkfile, fullpath);
            }
        }
        else {
            printf( "%s is unknown!\n", fullpath );
        }
    }
    closedir(dirp);
}