//  CITS2002 Project 2 2020
//  Name(s):             Jake Lorkin, Toby Digney
//  Student number(s):   22704986, 22385113

//                              MERGETARS
// USE: ./mergetars input_tarfile1 [input_tarfile2 ...] output_tarfile
// Takes a number of input tar or tar.gz files and merges them based on 
// last modified time and size. Produces merged output tar file 'output_tarfile'
//
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/param.h>
#include  <sys/stat.h>
#include  <time.h>
#include  <dirent.h>

#include "fileop.c"
#include "tarop.c"

