// Group Members: Ann Chapple, Nelson Paz, Noah Roden
// Instructor: Dr. Bo Sun
// Course: COSC 4302 (Operating Systems)
// Group Project: A Simple Shell
// Date: 4/30/2021

/**********************************************************************************
* Description: This shell is a simple shell designed in C. The code first prompts
* the user for a command. The prompt consists simply of "$".It then reads the 
* executable command in from the user, then finds it in PATH. The command is parsed 
* and then loaded and executed using the execv() function inside a child process.
* As such, basic Linux commands such as pwd, the ls commands, clear, and exit are 
* compatible with this program. The steps to the program can be described as
* follows:
*
* Program Steps 
* step1: Parse the path
* step2: Print the prompt
* step3: Read the command
* step4: Parse the command
* step5: Find the command
* step6: Execute the command
**********************************************************************************/

#define LINE_LEN 80            // The length of each command line.
#define MAX_ARGS 81            // The maximum amounts of arguments of each command.
#define MAX_ARG_LEN 16         // The length of each argument.
#define MAX_PATHS 64           // The maximum amounts of paths.
#define MAX_PATH_LEN 96        // The maximum length of each path.
#define WHITESPACE " .,''\t\n" // Delimeters for command line.
#define TRUE 1                 // 1 for TRUE
#ifndef NULL                   // If NULL is not defined, then define NULL as 0
#define NULL 0
#endif

// Each command line has a name,
// amount of arguments --> argc,
typedef struct {
    char *name;             // The command name
    int argc;               // The number of arguments
    char *argv[MAX_ARGS];   // The argument values (array of values)
} command_t;

// Header files
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Function prototypes
char *lookupPath(char **, char **);
int parseCommand(char *, command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);


/***************************************************************
 * Function main. This function performs the bulk of the
 * program's purpose: calling each of the seperate functions 
 * in the order of steps specified in the program description.
 * In short, the path is first parsed, the user is prompted for 
 * input a command is input by the user, the command is read, 
 * parsed, validated, and, if a valid command, executed by a 
 * child process using the execv function. The program closes 
 * when the user enters "exit" or "quit."
 **************************************************************/
int main(int argc, char *argv[]) {

    /*Initialize Variables */

    char *pathv[MAX_PATHS];                       // The array of paths
    command_t command;                            // Intialize command_t struct variable command
    char *commandLine = (char *)malloc(LINE_LEN); // The command line

    // Get the directory paths from PATH
    parsePath(pathv);

    // Print the first prompt
    printPrompt();

    // Loop the program to allow the user to input multiple commands
    // until the user wishes to exit
    while (TRUE) {

        // read the command from the keyboard, then store it into the commandLine.
        readCommand(commandLine);

        // if command is 'exit' or 'quit' then exit the program.
        if (strcasecmp(commandLine, "exit\n") == 0 || strcasecmp(commandLine, "quit\n") == 0) {
            free(commandLine);
            break;
        }

        // Parse the command
        parseCommand(commandLine, &command);

        // Assign the path to the command name
        command.name = lookupPath(command.argv, pathv);

        // Check if command is valid
        if (command.name == NULL) {
            printf("Command name is NULL! \n");
            printPrompt();
            continue;
        }

        // If command is valid, fork and execute command
        else {
            // create a child process.
            int childPid = fork();

            // Create temp and append path to command name
            char temp[LINE_LEN];
            strcpy(temp, "/bin/");
            strcat(temp, command.name);

            // If fork is successful
            if (childPid == 0) {
                // execute the command.
                execv(temp, argv);
            }

            // Wait for child to terminate
            if (wait(NULL) > 0) {
                printPrompt();
            }
        }
    }
    // Terminate the shell
    printf("Exiting...\n");
    return 0;
}

/**********************************************************
 * Function lookupPath. Searches the directories identified
 * by the dir argument to see if argv[0] (the file name)
 * appears there. Allocate a new string, place the full
 * path name in it, then return the string.
 *********************************************************/
char *lookupPath(char **argv, char **dir) {

    /*Initialize Variables*/
    char pName[MAX_PATH_LEN];
    int i;

    // Check to see if command name is already an absolute path name
    if (*argv[0] == '/') {
        if (access(argv[0], 0) == 0) {
            return argv[0];
        }
        return NULL;
    }

    // Look in PATH directories
    for (i = 0; i < MAX_PATHS; i++) {

        // Temporary absolute path
        char tpath[MAX_PATH_LEN];

        // If current directory is NULL, exit loop
        if (dir[i] == NULL) {
            break;
        }

        // Copy dir[i] to tpath
        strcpy(tpath, dir[i]);
        // Append '/' to tpath
        strcat(tpath, "/");
        strcat(tpath, argv[0]);
        strcpy(pName, tpath);

        // Use access() to see if the file is in a dir
        if (access(pName, 0) == 0) {
            return argv[0];
        }
    }

    return NULL;
}

/**********************************************************
 * Function parseCommand. Determines the command name and
 * constructs the parameter list. This function builds 
 * argv[] and sets the argc value, where argc is the
 * number of "tokens" or words on the command line. argv[]
 * is an array of strings (pointers to char *). The last
 * element in argv[] must be NULL. As we scan the command
 * line from the left, the first token goes into argv[0],
 * the second in argv[1], and so on. Each time we add a
 * token to argv[], we increment argc.
 *********************************************************/
int parseCommand(char *cLine, command_t *cmd) {

    /*Initialize Variables*/
    int argc;
    char **clPtr;
    clPtr = &cLine;
    argc = 0;

    // Initialize first index of cmd's array argv with MAX_ARG_LEN 
    cmd->argv[argc] = (char *)malloc(MAX_ARG_LEN);

    // Fill argv[] (we will split commands by whitespace)
    while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
        cmd->argv[++argc] = (char *)malloc(MAX_ARG_LEN);
        strcat(cmd->argv[0], cmd->argv[argc]);
    }

    // Set the command name and argc
    cmd->argc = argc - 1;
    cmd->name = (char *)malloc(sizeof(cmd->argv[0]));

    strcpy(cmd->name, cmd->argv[0]);

    return 1;
}

/**********************************************************
 * Function parsePath reads the PATH variable for this
 * environment, then builds an array, dirs[], of the
 * directories in PATH.
 *********************************************************/
int parsePath(char *dirs[]) {

    /*Initialize Variables*/
    char *pathEnvVar;
    char *thePath;
    int i;

    // Set all current directories to NULL
    for (i = 0; i < MAX_PATHS; i++) {
        dirs[i] = NULL;
    }

    // Store the value of PATH in pathEnvVar
    pathEnvVar = (char *)getenv("PATH");
    // Store the length + 1 of pathEnvVar in thePath 
    thePath = (char *)malloc(strlen(pathEnvVar) + 1);
    // Store pathEnvVar in thePath
    strcpy(thePath, pathEnvVar);


    for (i = 0; i < MAX_PATHS; i++) {
        // Split the value by ':', then store them into the dirs.
        dirs[i] = strsep(&thePath, ":");
        if (dirs[i] == NULL)
            break;
    }

    // Loop through all path directories
    for (i = 0; i < MAX_PATHS; i++) {
        // If current directory is null, finish
        if (dirs[i] == NULL) {
            break;
        }
    }
    return 0;
}

/**********************************************************
 * Function printPrompt. Prints the prompt which consists
 * simply of '$' for the user to enter a command.  
 *********************************************************/
void printPrompt() {
    const char *promptString = "$ ";
    printf("%s", promptString);
}

/**********************************************************
 * Function readCommand. Reads the command input by the
 * user into buffer using the standard input library.
 *********************************************************/
void readCommand(char *buffer) {
    fgets(buffer, sizeof(buffer), stdin);
}