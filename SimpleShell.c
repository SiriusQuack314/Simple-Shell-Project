// Group Members: Ann Chapple, Nelson Paz, Noah Roden
// Instructor: Dr. Bo Sun
// Course: COSC 4302 (Operating Systems)
// Group Project: A Simple Shell
// Date: 4/30/2021

/**********************************************************************************
* Description:
*
*
*
*
*
**********************************************************************************/

#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define MAX_LINE_LEN 80
#define WHITESPACE " .,\t\n"

#ifndef NULL
#define NULL
#endif

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
using namespace std;

struct command_t {
    char *name;
    int argc;
    char *argv[MAX_ARGS];
};

char *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);


/***********************************************
 * MAIN PROGRAM
 **********************************************/

int main() {

    /* Shell Initialization */

    int i;
    int pid, numChildren;
    int status;
    FILE *fid;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    /* Read the command line parameters */
    if (argc != 2) {
        fprintf(stderr, "Usage: launch <launch_set_filename>\n");
        exit(0);
    }

    /* Open a file that contains a set of commands */
    fid = fopen(argv[1], "r");

    /* Process each command in the launch file */
    numChildren = 0;
    while (fgets(cmdLine, MAX_LINE_LEN, fid) != NULL) {
        
        parseCommand(cmdLine, &command);
        command.argv[command.argc] = NULL;
        
        /* Create a child process to execute the command */
        if ((pid = fork()) == 0) {
            
            /* Child executing command*/
            execvp(command.name, command.argv); // IMPORTANT!!! This needs to be changed to use execv()
        }
        /* Parent continuing to the next command in the file */
        numChildren++;
    }
    cout << "\n\nlaunch: Launched %d commands\n", numChildren;

    /* Terminate after all children have terminated */
    for(i = 0; i < numChildren; i++) {
        wait(&status);
        /* Should free dynamic storage in command data structure */
    } 

    cout << "\n\nlaunch: Terminating successfully\n";
    return 0;
}


/*******************************************************
 * FUNCTIONS
 ******************************************************/

char *lookupPath(char **argv, char **dir) {
    /* This function searches the directories identified 
    by the dir argument to see if argv[0] (the file name)
    appears there. Allocate a new string, place the full
    path name in it, then return the string. */
    char *result;
    char pName[MAX_PATH_LEN];

    // Check to see if file name is already an absolute path name
    if (*argv[0] == '/') {
        //....code here
    }

    // Look in PATH directories.
    // Use access() to see if the file is in a dir.
    for (int i = 0; i < MAX_PATHS; i++) {
        //...code here
    }

    // File name not found in any path variable
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return NULL;
}


int parseCommand (char *cLine, struct command_t *cmd) {
    int argc;
    char **clPtr;
    /* Initialization */
    clPtr = &cLine;     /* cLine is the command line */
    argc = 0;
    cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
    /* Fill argv[] */
    while((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
        cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
    }

    /* Set the command name and argc */
    cmd->argc = argc-1;
    cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
    strcpy(cmd->name, cmd->argv[0]);
    return 1;
}

int parsePath(char *dirs[]) {
    /* This function reads the PATH variable for this 
    environment, then builds an array, dirs[], of the
    directories in PATH */
    char *pathEnvVar;
    char *thePath;

    for(int i = 0; i < MAX_ARGS; i++)
        dirs[i] = NULL;

    pathEnvVar = (char *) getenv("PATH");
    thePath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(thePath, pathEnvVar);

    /* Loop to parse thePath. Look for a ':' delimiter
     between each path name. */
}

void printPrompt () {
    /* Build the prompt string to have the machine name,
    current directory, or other desired information */
    string promptString = "__Our prompt here___";
    printf("%s, promptString");
}

void readCommand () {
    /* This code uses any set of I/O functions, such as 
    those in the stdio library to read the entire command
    line into the buffer. This implementation is greatly
    simplified, but it does the job. */ 
    gets(buffer);
}