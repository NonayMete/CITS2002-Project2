#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/*  CITS2002 Project 1 2020
    Name:                Jake Lorkin
    Student number(s):   22704986
 */

//  MAXIMUM NUMBER OF PROCESSES OUR SYSTEM SUPPORTS (PID=1..20)
#define MAX_PROCESSES 20

//  MAXIMUM NUMBER OF SYSTEM-CALLS EVER MADE BY ANY PROCESS
#define MAX_SYSCALLS_PER_PROCESS 50

//  MAXIMUM NUMBER OF PIPES THAT ANY SINGLE PROCESS CAN HAVE OPEN (0..9)
#define MAX_PIPE_DESCRIPTORS_PER_PROCESS 10

//  TIME TAKEN TO SWITCH ANY PROCESS FROM ONE STATE TO ANOTHER
#define USECS_TO_CHANGE_PROCESS_STATE 5

//  TIME TAKEN TO TRANSFER ONE BYTE TO/FROM A PIPE
#define USECS_PER_BYTE_TRANSFERED 1

//  ---------------------------------------------------------------------

enum state //Possible states a process can be in
{
    NEW,
    READY,
    RUNNING,
    EXITED,
    WAITING,
    SLEEPING,
    READBLOCKED,
    WRITEBLOCKED
};

enum sys_call //Possible system calls a process can make
{
    CALL_EXIT,
    CALL_COMPUTE,
    CALL_FORK,
    CALL_WAIT,
    CALL_SLEEP,
    CALL_PIPE,
    CALL_WRITE,
    CALL_READ
};

const char *state_descriptions[] = { //For debugging output
    "NEW",
    "READY",
    "RUNNING",
    "EXITED",
    "WAITING",
    "SLEEPING",
    "READBLOCKED",
    "WRITEBLOCKED"
};

int timetaken = 0;

//int current_PID = 1;
int PID_tracker[MAX_PROCESSES + 1];

//structs?
int current_process[MAX_PROCESSES + 1][MAX_SYSCALLS_PER_PROCESS][3]; //[PID][PID process number][array of system call info]
int process_tracker[MAX_PROCESSES + 1];

int sleep_tracker = __INT_MAX__;
int pipesize_bytes;
int timequantum_usecs;

int process_queue[MAX_PROCESSES + 1];
int front = 0;
int rear = -1;
int itemCount = 0;

int peek()
{
    return process_queue[front];
}

bool isFull()
{
    return itemCount == MAX_PROCESSES + 1;
}

bool isEmpty()
{
    return itemCount == 0;
}

void insert(int data)
{
    if (!isFull())
    {
        if (rear == MAX_PROCESSES)
        {
            rear = -1;
        }
        process_queue[++rear] = data;
        itemCount++;
    }
}

int removeData()
{
    int data = process_queue[front++];
    if (front == MAX_PROCESSES + 1)
    {
        front = 0;
    }
    itemCount--;
    return data;
}

//pipe structs
struct PipesOut
{
    bool active;
    int read_PID;
    int nbytes;
} pipe_out_tracker[MAX_PROCESSES + 1][MAX_PIPE_DESCRIPTORS_PER_PROCESS + 1];

struct PipesIn
{
    bool active;
    int write_PID;
    int nbytes;
} pipe_in_tracker[MAX_PROCESSES + 1][MAX_PIPE_DESCRIPTORS_PER_PROCESS + 1];

void add_process(int PID, int call, int arg0, int arg1)
{
    int process_number = process_tracker[PID];
    //printf("adding process PID:%i, call:%i process:%i\n", PID, call, process_number);
    process_tracker[PID]++;
    current_process[PID][process_number][0] = call;
    current_process[PID][process_number][1] = arg0;
    current_process[PID][process_number][2] = arg1;
}

void change_state(int PID, int state)
{
    printf("@%i P%i: %s -> %s\n", timetaken, PID, state_descriptions[PID_tracker[PID]], state_descriptions[state]);
    PID_tracker[PID] = state;
    timetaken += USECS_TO_CHANGE_PROCESS_STATE;
}

//  ---------------------------------------------------------------------
void sim_exit(int PID) //Simulate the exit() system call, terminates process and closes pipes.
{
    printf("P%i:EXITED\n", PID);
    change_state(PID, EXITED);
    //close pipes
    for(int i = 0; i < MAX_PIPE_DESCRIPTORS_PER_PROCESS+1; i++) {
        pipe_out_tracker[PID][i].active = false;
        pipe_in_tracker[PID][i].active = false;
    }
}

void sim_compute(int PID, int time)
{
    if (time <= timequantum_usecs)
    {
        printf("P%i:COMPUTE for %i now complete\n", PID, time);
        timetaken += time;
        process_tracker[PID]++;
    }
    else
    {
        time -= timequantum_usecs;
        printf("P%i:COMPUTE for %i, %i left to go\n", PID, timequantum_usecs, time);
        timetaken += timequantum_usecs;
        current_process[PID][process_tracker[PID]][1] = time;
    }
    change_state(PID, READY);
    insert(PID);
}

void sim_fork(int PID, int childPID)
{
    printf("P%i:FORK new childPID: %i\n", PID, childPID);
    change_state(childPID, READY);

    //check if parent process has any active but unassigned pipes
    for (int i = 0; i < MAX_PIPE_DESCRIPTORS_PER_PROCESS + 1; i++)
    {
        if (pipe_out_tracker[PID][i].active && pipe_out_tracker[PID][i].read_PID == 0)
        {
            //assign child to this pipe
            pipe_out_tracker[PID][i].read_PID = childPID;
            //assign this pipe to child
            pipe_in_tracker[childPID][i].active = true;
            pipe_in_tracker[childPID][i].write_PID = PID;
            pipe_in_tracker[childPID][i].nbytes = pipe_out_tracker[PID][i].nbytes;
            printf("P%i/%i => [%i] => P%i/%i\n", PID, i, pipe_out_tracker[PID][i].nbytes, childPID, i);
        }
    }

    change_state(PID, READY);
    insert(childPID);
    insert(PID);
    process_tracker[PID]++;
}

void sim_wait(int PID, int childPID)
{
    if (PID_tracker[PID] != WAITING)
    {
        printf("P%i:WAITING for childPID: %i\n", PID, childPID);
        change_state(PID, WAITING);
    }
    if (PID_tracker[childPID] == EXITED)
    {
        printf("P%i has been WAITING for childPID: %i\n", PID, childPID);
        change_state(PID, READY);
        process_tracker[PID]++;
    }
    insert(PID);
}

void sim_sleep(int PID, int time)
{
    if (PID_tracker[PID] != SLEEPING)
    {
        printf("P%i:SLEEPING for %i us\n", PID, time);
        change_state(PID, SLEEPING);
        current_process[PID][process_tracker[PID]][1] += timetaken;
    }

    int temp = current_process[PID][process_tracker[PID]][1];
    if (temp < sleep_tracker)
        sleep_tracker = temp;

    if (current_process[PID][process_tracker[PID]][1] - timetaken <= 0)
    {
        printf("P%i:finished sleeping\n", PID);
        change_state(PID, READY);
        process_tracker[PID]++;
        sleep_tracker = __INT_MAX__;
    }
    insert(PID);
}

void sim_pipe(int PID, int desc)
{
    printf("P%i:PIPE desc:%i, ", PID, desc);
    //initiate PID as the writing end of pipe desc
    pipe_out_tracker[PID][desc].active = true; //get pipe ready for assignment when fork() is called
    process_tracker[PID]++;
    change_state(PID, READY);
    printf("P%i/%i => [%i] => none\n", PID, desc, pipe_out_tracker[PID][desc].nbytes);
    insert(PID);
}

void sim_write(int PID, int desc, int bytes)
{
    //simulate writing 'bytes' bytes to pipe desc from PID
    printf("P%i:WRITEPIPE %i bytes to desc %i\n", PID, bytes, desc);
    int nbytes = pipe_out_tracker[PID][desc].nbytes;
    pipe_out_tracker[PID][desc].nbytes += bytes;
    if (pipe_out_tracker[PID][desc].nbytes > pipesize_bytes)
    {
        timetaken += pipesize_bytes - nbytes;
        current_process[PID][process_tracker[PID]][2] -= pipesize_bytes - nbytes;
        pipe_out_tracker[PID][desc].nbytes = pipesize_bytes;
        change_state(PID, WRITEBLOCKED);
    }
    else
    {
        process_tracker[PID]++;
        timetaken += USECS_PER_BYTE_TRANSFERED * bytes;
        change_state(PID, READY);
        insert(PID);
    }

    int read_PID = pipe_out_tracker[PID][desc].read_PID;
    pipe_in_tracker[read_PID][desc].nbytes = pipe_out_tracker[PID][desc].nbytes;

    printf("P%i/%i => [%i] => P%i/%i\n", PID, desc, pipe_out_tracker[PID][desc].nbytes, read_PID, desc);

    //unlock any processes waiting to read
    if (PID_tracker[read_PID] == READBLOCKED && current_process[read_PID][process_tracker[read_PID]][1] == desc)
    {
        printf("P%i: can read its pipedesc=%i\n", read_PID, desc);
        change_state(read_PID, READY);
        insert(read_PID);
    }
    
}

void sim_read(int PID, int desc, int bytes)
{
    //simulate reading 'bytes' bytes from pipe desc by PID
    printf("P%i:READPIPE %i bytes from desc %i\n", PID, bytes, desc);
    int nbytes = pipe_in_tracker[PID][desc].nbytes;
    int write_PID = pipe_in_tracker[PID][desc].write_PID;
    if (nbytes >= bytes)
    {
        pipe_in_tracker[PID][desc].nbytes -= bytes;
        pipe_out_tracker[write_PID][desc].nbytes -= bytes;
        timetaken += USECS_PER_BYTE_TRANSFERED * bytes;
        change_state(PID, READY);
        process_tracker[PID]++;
        insert(PID);
    }
    else
    { //not enough bytes to read
        current_process[PID][process_tracker[PID]][2] -= nbytes;
        pipe_in_tracker[PID][desc].nbytes = 0;
        pipe_out_tracker[write_PID][desc].nbytes = 0;
        timetaken += USECS_PER_BYTE_TRANSFERED * nbytes;
        change_state(PID, READBLOCKED);
    }
    printf("P%i/%i => [%i] => P%i/%i\n", write_PID, desc, pipe_out_tracker[PID][desc].nbytes, PID, desc);

    //unlock any processes waiting to write
    if (PID_tracker[write_PID] == WRITEBLOCKED && current_process[write_PID][process_tracker[write_PID]][1] == desc)
    {
        printf("P%i: can write to pipedesc=%i\n", write_PID, desc);
        change_state(write_PID, READY);
        insert(write_PID);
    }
}

void run_simulation()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_tracker[i] = 0;
    }
    printf("Booting PID = 1\n");
    insert(1);
    PID_tracker[1] = RUNNING;

    int old_timetaken = 0;
    bool finished = false;
    while (!isEmpty())
    {
        int current_PID = removeData();
        if (finished)
            break;
        if (PID_tracker[current_PID] == READY)
        {
            //printf("ready\n");
            change_state(current_PID, RUNNING);
        }
        else if (PID_tracker[current_PID] == SLEEPING)
        {
            if (old_timetaken == timetaken)
            {
                timetaken = sleep_tracker;
            }
            old_timetaken = timetaken;
        }
        else if (PID_tracker[current_PID] == READBLOCKED || PID_tracker[current_PID] == WRITEBLOCKED)
        {
            insert(current_PID);
            continue;
        } else if(PID_tracker[current_PID] == EXITED)
        {
            continue;
        }

        int call = current_process[current_PID][process_tracker[current_PID]][0];
        int arg0 = current_process[current_PID][process_tracker[current_PID]][1];
        int arg1 = current_process[current_PID][process_tracker[current_PID]][2];
        //printf("process track: %i\n", process_tracker[current_PID]);
        //printf("pid track %i\n", PID_tracker[current_PID]);
        //printf("call %i\n", call);
        if (call == CALL_EXIT)
        {
            sim_exit(current_PID);
        }
        else if (call == CALL_COMPUTE)
        {
            sim_compute(current_PID, arg0);
        }
        else if (call == CALL_FORK)
        {
            sim_fork(current_PID, arg0);
        }
        else if (call == CALL_WAIT)
        {
            sim_wait(current_PID, arg0);
        }
        else if (call == CALL_SLEEP)
        {
            sim_sleep(current_PID, arg0);
        }
        else if (call == CALL_PIPE)
        {
            sim_pipe(current_PID, arg0);
        }
        else if (call == CALL_WRITE)
        {
            sim_write(current_PID, arg0, arg1);
        }
        else if (call == CALL_READ)
        {
            sim_read(current_PID, arg0, arg1);
        }
        current_PID++; //set up max checker
    }

    printf("HALT\n");
}

//  ---------------------------------------------------------------------

//  FUNCTIONS TO VALIDATE FIELDS IN EACH eventfile - NO NEED TO MODIFY
int check_PID(char word[], int lc)
{
    int PID = atoi(word);

    if (PID <= 0 || PID > MAX_PROCESSES)
    {
        printf("invalid PID '%s', line %i\n", word, lc);
        exit(EXIT_FAILURE);
    }
    return PID;
}

int check_microseconds(char word[], int lc)
{
    int usecs = atoi(word);

    if (usecs <= 0)
    {
        printf("invalid microseconds '%s', line %i\n", word, lc);
        exit(EXIT_FAILURE);
    }
    return usecs;
}

int check_descriptor(char word[], int lc)
{
    int pd = atoi(word);

    if (pd < 0 || pd >= MAX_PIPE_DESCRIPTORS_PER_PROCESS)
    {
        printf("invalid pipe descriptor '%s', line %i\n", word, lc);
        exit(EXIT_FAILURE);
    }
    return pd;
}

int check_bytes(char word[], int lc)
{
    int nbytes = atoi(word);

    if (nbytes <= 0)
    {
        printf("invalid number of bytes '%s', line %i\n", word, lc);
        exit(EXIT_FAILURE);
    }
    return nbytes;
}

//  parse_eventfile() READS AND VALIDATES THE FILE'S CONTENTS
//  YOU NEED TO STORE ITS VALUES INTO YOUR OWN DATA-STRUCTURES AND VARIABLES
void parse_eventfile(char program[], char eventfile[])
{
#define LINELEN 100
#define WORDLEN 20
#define CHAR_COMMENT '#'

    //  ATTEMPT TO OPEN OUR EVENTFILE, REPORTING AN ERROR IF WE CAN'T
    FILE *fp = fopen(eventfile, "r");

    if (fp == NULL)
    {
        printf("%s: unable to open '%s'\n", program, eventfile);
        exit(EXIT_FAILURE);
    }

    char line[LINELEN], words[4][WORDLEN];
    int lc = 0;

    //  READ EACH LINE FROM THE EVENTFILE, UNTIL WE REACH THE END-OF-FILE
    while (fgets(line, sizeof line, fp) != NULL)
    {
        ++lc;

        //  COMMENT LINES ARE SIMPLY SKIPPED
        if (line[0] == CHAR_COMMENT)
        {
            continue;
        }

        //  ATTEMPT TO BREAK EACH LINE INTO A NUMBER OF WORDS, USING sscanf()
        int nwords = sscanf(line, "%19s %19s %19s %19s",
                            words[0], words[1], words[2], words[3]);

        //  WE WILL SIMPLY IGNORE ANY LINE WITHOUT ANY WORDS
        if (nwords <= 0)
        {
            continue;
        }

        //  ENSURE THAT THIS LINE'S PID IS VALID
        int thisPID = check_PID(words[0], lc);

        //  OTHER VALUES ON (SOME) LINES
        int otherPID, nbytes, usecs, pipedesc;

        //  IDENTIFY LINES RECORDING SYSTEM-CALLS AND THEIR OTHER VALUES
        //  THIS FUNCTION ONLY CHECKS INPUT;  YOU WILL NEED TO STORE THE VALUES
        if (nwords == 3 && strcmp(words[1], "compute") == 0)
        {
            usecs = check_microseconds(words[2], lc);
            add_process(thisPID, CALL_COMPUTE, usecs, 0);
        }
        else if (nwords == 3 && strcmp(words[1], "sleep") == 0)
        {
            usecs = check_microseconds(words[2], lc);
            add_process(thisPID, CALL_SLEEP, usecs, 0);
        }
        else if (nwords == 2 && strcmp(words[1], "exit") == 0)
        {
            add_process(thisPID, CALL_EXIT, 0, 0);
        }
        else if (nwords == 3 && strcmp(words[1], "fork") == 0)
        {
            otherPID = check_PID(words[2], lc);
            add_process(thisPID, CALL_FORK, otherPID, 0);
        }
        else if (nwords == 3 && strcmp(words[1], "wait") == 0)
        {
            otherPID = check_PID(words[2], lc);
            add_process(thisPID, CALL_WAIT, otherPID, 0);
        }
        else if (nwords == 3 && strcmp(words[1], "pipe") == 0)
        {
            pipedesc = check_descriptor(words[2], lc);
            add_process(thisPID, CALL_PIPE, pipedesc, 0);
        }
        else if (nwords == 4 && strcmp(words[1], "writepipe") == 0)
        {
            pipedesc = check_descriptor(words[2], lc);
            nbytes = check_bytes(words[3], lc);
            add_process(thisPID, CALL_WRITE, pipedesc, nbytes);
        }
        else if (nwords == 4 && strcmp(words[1], "readpipe") == 0)
        {
            pipedesc = check_descriptor(words[2], lc);
            nbytes = check_bytes(words[3], lc);
            add_process(thisPID, CALL_READ, pipedesc, nbytes);
        }
        //  UNRECOGNISED LINE
        else
        {
            printf("%s: line %i of '%s' is unrecognized\n", program, lc, eventfile);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);

#undef LINELEN
#undef WORDLEN
#undef CHAR_COMMENT
}

//  ---------------------------------------------------------------------

//  CHECK THE COMMAND-LINE ARGUMENTS, CALL parse_eventfile(), RUN SIMULATION
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Incorrect arguments given\n");
        exit(EXIT_FAILURE);
    }

    char *eventfile = argv[1];
    timequantum_usecs = atoi(argv[2]);
    pipesize_bytes = atoi(argv[3]);

    if (timequantum_usecs <= 0 || pipesize_bytes <= 0)
    {
        printf("Incorrecr arguments given\n");
        exit(EXIT_FAILURE);
    }
    printf("file is %s\n", eventfile);
    parse_eventfile(argv[0], eventfile);
    //printf("%i , %i , %i, %i\n", queue[0][0], queue[0][1], queue[0][2], queue[0][3]);
    run_simulation();
    //printf("PID tracker: %i , %i , %i, %i\n", PID_tracker[0], PID_tracker[1], PID_tracker[2], PID_tracker[3]);

    printf("timetaken %i\n", timetaken);
    return 0;
}