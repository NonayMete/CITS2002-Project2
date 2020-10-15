// FILE AND DIRECTORY RELATED FUNCTIONS

char *progname;
/*
*   Creates a temporary folder in directory 'TEMPLATE'.
*   Folder name is randomised. Returns pointer to directory of folder
*/
#define TEMPLATE "/tmp/mt-XXXXXX"
char *create_temp_directory()
{
    char newdirname[] = TEMPLATE;
    char *tempdir = malloc(MAXPATHLEN);
    mkdtemp(newdirname);
    strcpy(tempdir, newdirname);
    printf("Created temp directory: %s\n", tempdir);
    return tempdir;
}

/*
*   Moves file 'in_file' to directory 'out_dir'
*/
void move_file(char *in_file, char *out_dir)
{
    char *infile = malloc(MAXPATHLEN);
    strcpy(infile, in_file);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_dir);

    pid_t pid = fork();
    if (pid == -1)
    {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char *args[] = {"mv", infile, outfile, NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/*
*   Removes file or directory 'dir_' and everything inside it
*/
void remove_directory(char *dir_)
{
    char dir[MAXPATHLEN];
    strcpy(dir, dir_);
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("didn't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char *args[] = {"rm", "-rf", dir, NULL};
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/*
*   Returns a struct containing metadata about specified file 'filename'
*/
struct stat file_attributes(char *filename)
{
    struct stat stat_buffer;
    if (stat(filename, &stat_buffer) != 0)
    {
        perror(progname);
        exit(EXIT_FAILURE);
    }
    return stat_buffer;
}

/*
*   Compares metadata about 2 files and returns the one which takes priority.
*/
char *compare_files(char *file1_, char *file2_)
{
    struct stat file1 = file_attributes(file1_);
    struct stat file2 = file_attributes(file2_);
    if ((int)file1.st_mtime > (int)file2.st_mtime)
    {
        return file1_;
    }
    else if ((int)file1.st_mtime < (int)file2.st_mtime)
    {
        return file2_;
    }

    if ((int)file1.st_size > (int)file2.st_size)
    {
        return file1_;
    }
    else if ((int)file1.st_size < (int)file2.st_size)
    {
        return file2_;
    }
    return file2_;
}

/*
*   Recursively propagates through all files and directories inside 'dir_name'
*   adds all files and directories to 'out_file'. If file already exists inside 
*   'out_file' then compare_files() is called to decide which one is used.
*/
void merge_directories(char *dir_name, char *out_file, char *parent_file)
{
    char *dirname = malloc(MAXPATHLEN);
    strcpy(dirname, dir_name);
    char *outfile = malloc(MAXPATHLEN);
    strcpy(outfile, out_file);

    char fullpath[MAXPATHLEN];
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(dirname);
    if (dirp == NULL)
    {
        perror(progname);
        exit(EXIT_FAILURE);
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        struct stat stat_buffer;
        sprintf(fullpath, "%s/%s", dirname, dp->d_name);
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
        {
            continue;
        }
        if (stat(fullpath, &stat_buffer) != 0)
        {
            perror(progname);
        }
        else if (S_ISDIR(stat_buffer.st_mode))
        {
            //printf( "%s is a directory\n", fullpath );
            char newdir[MAXPATHLEN];
            sprintf(newdir, "%s/%s%s", outfile, parent_file, dp->d_name);
            DIR *dir = opendir(newdir);
            if (dir)
            {
                closedir(dir);
            }
            else
            {
                //directory doesnt exist create it
                mkdir(newdir, 0700);
            }
            char parent[MAXPATHLEN];
            sprintf(parent, "%s%s/", parent_file, dp->d_name);
            merge_directories(fullpath, outfile, parent);
        }
        else if (S_ISREG(stat_buffer.st_mode))
        {
            //printf( "%s is a regular file\n", fullpath );

            //check if file exists
            char checkfile[MAXPATHLEN];
            sprintf(checkfile, "%s/%s%s", outfile, parent_file, dp->d_name);
            //printf("checkfile: %s\n", checkfile);
            char newfile[MAXPATHLEN];
            sprintf(newfile, "%s/%s", outfile, parent_file);
            if (access(checkfile, F_OK) != 0)
            {
                //if not move the file there
                move_file(fullpath, newfile);
            }
            else
            {
                //if it does exist then preform checks to see which one wins
                //printf("%s already exists not moving\n", checkfile);
                char *pref_file = malloc(MAXPATHLEN);
                pref_file = compare_files(checkfile, fullpath);
                move_file(pref_file, newfile);
            }
        }
        else
        {
            printf("%s is unknown!\n", fullpath);
        }
    }
    closedir(dirp);
}