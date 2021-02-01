/*
Joseph Polaski
CS344: Program 3

smallsh.c

This is the main source file for the smallsh shell program. 

*/

#define _GNU_SOURCE // access nonstandard GNU/Linux functions
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "smallsh.h"
#include "expansion.h"

// Global Constants
#define MAXCMDLEN 2048 // maximum characters allowed in command line
#define MAXARGS 512 // maximum arguments allowed in command line

// This struct will be used to keep track of the user input commands
struct userCommands
{
    char *cmdWithArgs[MAXARGS]; // will be an array of strings with the command and all arguments
    char *inputFile; // will hold input file name if any
    char *outputFile; // will hold output file if any
    bool isBackground; // will the process run in the background?
};

int main(void)
{       

    char *userCmdLine; // will hold the original command line from the user max length 2048 characters

    // Establish Main Loop Flag
    enum progStatus {active, inactive};
    enum progStatus shellStatus = active;

    // initiate main loop
    while(shellStatus == active)
    {   
        // get command line from the user
        userCmdLine = getUserCommandLine(MAXCMDLEN);

        // process user command line into organized struct
        struct userCommands *userEntry = buildCmdStruct(userCmdLine);

        printStruct(userEntry);

        // execute user commands
        shellStatus = runUserCommands(userEntry);

        

        // remember to free struct
        free(userCmdLine);
    }

    return 0;
}

/*
*   getUserCommandLine
*
*   Reads in the commandline entered by the user into the shell
*
*   :parameter: maxLength: 2048 characters is the maximum command line length allowed
*   :return: cmdLine: The command line entered by the user
*/
char *getUserCommandLine(int maxLength)
{
    char *cmdLine = calloc(maxLength, sizeof(char)); // dynamically allocate memory for user input on the heap

    printf(": "); // print out smallsh prompt token to user
    fflush(stdout); // flush

    fgets(cmdLine, maxLength, stdin); // read in command line from user
    cmdLine[strlen(cmdLine)-1] = '\0'; // strip off \n from fgets

    return (cmdLine);
}

/*
*   buildCmdStruct
*
*   takes the raw user entered command line and builds it int an organized struct
*
*   :parameter: userCmdLine: command line entered by the user
*   :return: cmdStruct: structure containing commands organized as data members
*/
struct userCommands *buildCmdStruct(char *userCmdLine)
{       
    // dynamically allocate memory for return structure
    struct userCommands *cmdStruct = malloc(sizeof(struct userCommands));

    int argIndex = 0; // will track the argument index for cmdStruct->cmdWIthArgs[]

    // initialize optional data members
    cmdStruct->inputFile = NULL;
    cmdStruct->outputFile = NULL;
    cmdStruct->isBackground = false;

    // tokenize userCmdLine and read in all values
    char *token;

    // retrieve first token
    token = strtok(userCmdLine, " ");

    // keep parsing " " delimited strings until the end of the command line
    while(token != NULL)
    {   
        // check for input file
        if(strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " "); // skip to input file name
            cmdStruct->inputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string and '\0'
            strcpy(cmdStruct->inputFile, token); // copy string to inputFile data member
        }
        // check for output file
        else if(strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " "); // skip to output file name
            cmdStruct->outputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string and '\0'
            strcpy(cmdStruct->outputFile, token); // copy string to outputFile data member
        }
        // check for background flag
        else if(strcmp(token, "&") == 0)
        {
            cmdStruct->isBackground = true; // set background flag to true
        }
        // treat as command or argument
        else
        {   
            char *resultString; // to hold result of expand$$()            
            resultString = expand$$(token); // check token for expansion and retrieve expanded form of string or original string

            // add to command/arguments array
            cmdStruct->cmdWithArgs[argIndex] = calloc(strlen(resultString) + 1, sizeof(char)); // dynamically allocate memory for token size string and '\0'
            strcpy(cmdStruct->cmdWithArgs[argIndex], resultString); // copy to array

            argIndex++; // increment insertion index of command/arguments array            
        }

        token = strtok(NULL, " "); // retreive next token
    }

    return cmdStruct;    
}

/*
*   runUserCommands
*
*   This function executes whatever commands the user provided to smallsh
*/
int runUserCommands(struct userCommands *cmdStruct)
{
    // check custom commands first
    if(strcmp(cmdStruct->cmdWithArgs[0], "exit") == 0)
    {
        // exit status give, set flag to inactive (1)
        return 1;
    }
    else if(strcmp(cmdStruct->cmdWithArgs[0], "cd") == 0)
    {
        char *pathArg = cmdStruct->cmdWithArgs[1]; // fetch single argument supported by custom cd

        // if the user supplied a path argument
        if(pathArg != NULL)
        {
            chdir(pathArg); // change directory to user specified path
        }
        // user supplied no path
        else
        {   
            chdir(getenv("HOME")); // change directory to path specified in HOME environment variable
        }
    }
    else if(strcmp(cmdStruct->cmdWithArgs[0], "status") == 0)
    {
        // not 100% on this yet
    }
    // Execute all other non - custom commands
    else
    {
        executeOthers(cmdStruct);
    }
    
    return 0;
}

/*
*   executeOthers
*
*   This function forks a new child process and exits 
*   any non custom linux shell commands provided.
*
*   Source Cited:
*   title: Exploration: Process API - Monitoring Child Processes
*   author: OSU Instructor - Unknown
*   URL: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-monitoring-child-processes?module_item_id=20163874
*   Description: I took the code from the wait repl.it example code and used it and modified it to work for the purposes
*   of my program. 
*/
void executeOthers(struct userCommands *cmdStruct)
{
    pid_t spawnpid = -5; // initialize with non standard value (garbage)
	int childStatus; // will contain child exit status
    int childPid; // child pid will be returned by wait

    // fork child process
	spawnpid = fork();

	switch (spawnpid){
		case -1:
			perror("forking child process failed!");
            fflush(stdout); // flush stdout
			exit(1);
			break;
		case 0:
            // Runs in Child Process
			
            redirectIO(cmdStruct); // setup file redirection

            execvp(cmdStruct->cmdWithArgs[0], cmdStruct->cmdWithArgs); // execute command

            perror("smallsh: command not found!");
			break;
		default:
            // Runs in Parent Process
            // if foreground wait for process
            childPid = wait(&childStatus);
            
			break;
	}                                                                        
}

// redirects file IO
void redirectIO(struct userCommands *cmdStruct)
{   
    int fileDesc[2]; // use for file descriptors

    // check for input file
    if(cmdStruct->inputFile != NULL)
    {
        fileDesc[0] = open(cmdStruct->inputFile, O_RDONLY | O_CLOEXEC, 0700); // open input file for read only set to close by exec functions

        // check for error
        if(fileDesc[0] == -1)
        {
            perror("Open input file error: "); // opening file failed
        }
        else
        {
             dup2(fileDesc[0], 0); // redirect stdin to the given input file
        }       
    }

    // check for outnput file
    if(cmdStruct->outputFile != NULL)
    {
        fileDesc[1] = open(cmdStruct->outputFile, O_CREAT | O_WRONLY | O_APPEND | O_CLOEXEC, 0700); // open output file for read only set to close by exec functions
        
        // check for error
        if(fileDesc[1] == -1)
        {
            perror("Open input file error: "); // opening file failed
        }
        else
        {
            dup2(fileDesc[1], 1); // redirect stdout to the given output file
        }       
    }
}

// prints struct data members for testing
void printStruct(struct userCommands *testStruct)
{   
    printf("Input File: %s\n", testStruct->inputFile);
    printf("Output File: %s\n", testStruct->outputFile);
    printf(testStruct->isBackground ? "isBackground: true\n" : "isBackground: false\n");

    for(int i = 0; i < MAXARGS; i++){
        if(testStruct->cmdWithArgs[i] != NULL)
        {
            printf("cmdArg %i: %s\n", i, testStruct->cmdWithArgs[i]);
        }
    }
}