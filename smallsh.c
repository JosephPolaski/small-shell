/*
Joseph Polaski
CS344: Program 3

smallsh.c

This is the main source file for the smallsh shell program. 

Source Cited:
Title: Exploration: Signal Handling API
Author: Unknown OSU Instructor
URL: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
Description: I heavily referenced this exploration module and used a modified version of the 
code for my own application of custom signal handlers.

*/

#define _GNU_SOURCE // access nonstandard GNU/Linux functions
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "expansion.h"
#include "smallsh.h"

// Global Constants
#define MAXCMDLEN 2048 // maximum characters allowed in command line
#define MAXARGS 512 // maximum arguments allowed in command line
#define BGPROCS 300 // length of background proccess tracking list

bool foreground_only = false; // global foreground flag

// This struct will be used to keep track of the user input commands
struct userCommands
{
    char *cmdWithArgs[MAXARGS + 1]; // will be an array of strings with the command and all 512 arguments
    char *inputFile; // will hold input file name if any
    char *outputFile; // will hold output file if any
    bool isBackground; // will the process run in the background?
    bool hasRedir; // is there any IO redirection in the command
};

int main(void)
{       
    char *userCmdLine; // will hold the original command line from the user max length 2048 characters
    int lastFGProcStat = 0; // will contiain the exit status of the last run foreground process
    int backgroundProcs[BGPROCS]; // will be used to store background process pids

    // initialize background pid array to all 0 values
    for(int i = 0; i < BGPROCS; i ++)
        backgroundProcs[i] = 0;  

    // declare empty action struct for Ctrl-C (SIGINT)
    struct sigaction SIGINT_action = {0};
   
    // configure struct
	SIGINT_action.sa_handler = SIG_IGN;  // set handler to ignore SIGINT
	sigfillset(&SIGINT_action.sa_mask); // set mask for signal blocking 
	SIGINT_action.sa_flags = 0; // set no flags
	sigaction(SIGINT, &SIGINT_action, NULL); // register custom ignore handler to SIGINT signal 

    // declare empty action struct for Ctrl-Z (SIGTSTP)
    struct sigaction SIGTSTP_action = {0};
   
    // configure struct
	SIGTSTP_action.sa_handler = SIGTSTP_Handler;  // set handler to ignore SIGINT
	sigfillset(&SIGTSTP_action.sa_mask); // set mask for signal blocking 
	SIGTSTP_action.sa_flags = 0; // set no flags
	sigaction(SIGTSTP, &SIGTSTP_action, NULL); // register custom ignore handler to SIGINT signal

    // Establish Main Loop Flag
    enum progStatus {active, inactive};
    enum progStatus shellStatus = active;

    // initiate main loop
    while(shellStatus == active)
    {   
        // check for any finishing background processes
        checkBackground(backgroundProcs);           

        // get command line from the user
        userCmdLine = getUserCommandLine(MAXCMDLEN);

        // process user command line into organized struct
        struct userCommands *userEntry = buildCmdStruct(userCmdLine);

        // execute user commands
        shellStatus = runUserCommands(userEntry, &lastFGProcStat, SIGINT_action, backgroundProcs);        

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

    
    // initialize array to all NULL values
    for(int i = 0; i < (MAXARGS + 1); i++)
    {
        cmdStruct->cmdWithArgs[i] = NULL;
    }
    // initialize optional data members
    cmdStruct->inputFile = NULL;
    cmdStruct->outputFile = NULL;
    cmdStruct->isBackground = false;
    cmdStruct->hasRedir = false;

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
            cmdStruct->hasRedir = true; // set redirection flag
            token = strtok(NULL, " "); // skip to input file name
            cmdStruct->inputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string and '\0'
            strcpy(cmdStruct->inputFile, token); // copy string to inputFile data member
        }
        // check for output file
        else if(strcmp(token, ">") == 0)
        {
            cmdStruct->hasRedir = true; // set redirection flag
            token = strtok(NULL, " "); // skip to output file name
            cmdStruct->outputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string and '\0'
            strcpy(cmdStruct->outputFile, token); // copy string to outputFile data member
        }
        // check for background flag
        else if(strcmp(token, "&") == 0)
        {   
            // allow background only outside of foreground mode
            if(foreground_only == false)
            {
                cmdStruct->isBackground = true; // set background flag to true
            }            
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
*   :parameter: cmdStruct - the structure holding the user entered command data
*   :parameter: lastProcState - used to return the exit status for the last child process
*   :parameter: sigIntAction - this is the signal handler structure for SIGINT (ctrl-c)
*   :parameter: backgroundPIDs[] - this is an array used to keep track of background PIDs
*
*   This function executes whatever commands the user provided to smallsh if the command is unknown in give an error
*/
int runUserCommands(struct userCommands *cmdStruct, int *lastProcStat, struct sigaction sigIntAction, int backgroundPIDs[])
{
    // check custom commands first
    if(cmdStruct->cmdWithArgs[0] == NULL || strcmp(cmdStruct->cmdWithArgs[0], "#") == 0 || cmdStruct->cmdWithArgs[0][0] == '#')
    {   
        // comment or NULL was entered, Do Nothing
        return 0;
    }
    else if(strcmp(cmdStruct->cmdWithArgs[0], "exit") == 0)
    {  
        fflush(stdout);
        char *exitTxt = "...Killing all child processes and exiting smallsh shell\n"; // custom exit message
        write(STDOUT_FILENO, exitTxt, strlen(exitTxt));
        
        killpg(getpgrp(), SIGTERM); // kill entire process group of shell, SIGTERM allows for cleanup
        exit(0); // exit shell
    }
    else if(strcmp(cmdStruct->cmdWithArgs[0], "cd") == 0)
    {
        char *pathArg = cmdStruct->cmdWithArgs[1]; // fetch single argument supported by custom cd

        // if the user supplied a path argument
        if(pathArg != NULL)
        {
           // change directory to user specified path
           if(chdir(pathArg) != 0)
            {
                perror("Changing Directories Failed"); // print error if failure
            } 
        }
        // user supplied no path
        else
        {               
            chdir(getenv("HOME")); // change directory to path specified in HOME environment variable         
        }
    }
    else if(strcmp(cmdStruct->cmdWithArgs[0], "status") == 0)
    {
        checkExitStatus(*lastProcStat); // check the exit status of the last forground process
    }
    // Execute all other non - custom commands
    else
    {
        executeOthers(cmdStruct, lastProcStat, sigIntAction, backgroundPIDs);
    }
    
    return 0;
}

/*
*   executeOthers
*
*   :parameter: lastProcState - used to return the exit status for the last child process
*   :parameter: sigIntAction - this is the signal handler structure for SIGINT (ctrl-c)
*   :parameter: backgroundPIDs[] - this is an array used to keep track of background PIDs
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
void executeOthers(struct userCommands *cmdStruct, int *lastProcStat, struct sigaction sigIntAction, int backgroundPIDs[])
{
    pid_t spawnpid = -5; // initialize with non standard value (garbage)
	int childStatus; // will contain child exit status
    int childPid; // child pid will be returned by waitpid
    int bgChildPid; // for background child pids
    int execResult = 0; // will hold the result of the execvp()

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
            
            // restore normal Ctrl-C function for foreground children
            if(cmdStruct->isBackground == false)
            {
                sigIntAction.sa_handler = SIG_DFL; // restore SIGINT to default action for child
                sigaction(SIGINT, &sigIntAction, NULL); // register default handler to SIGINT signal
            }
                   
            redirectIO(cmdStruct); // setup file redirection                    

            execResult = execvp(cmdStruct->cmdWithArgs[0], cmdStruct->cmdWithArgs); // execute command
                        // check for execution failure
            if(execResult == -1)
            {
                perror("smallsh: command not found!");
                exit(1); // exit on failure
            }            
			break;
		default:
            // Runs in Parent Process
            // Check Background Flag
            if(cmdStruct->isBackground == false)
            {   
                // Run Foreground Process
                childPid = waitpid(spawnpid, &childStatus, 0); // BG flag == false, run process in foreground
                *lastProcStat = childStatus; // return to lastFGProc in main() for use with status()

                 // check for signal termination by ctrl-c and print messsage
                if(WIFSIGNALED(childStatus) != 0 && childStatus == SIGINT)
                {
                    // print out signal number to user
                    printf("foreground process terminated by signal: %i\n", WTERMSIG(childStatus));
                    fflush(stdout);
                }
            }
            else
            {   
                // Run Background Process
                bgChildPid = waitpid(spawnpid, &childStatus, WNOHANG); // BG flag == true, run process in background
                trackBGPID(spawnpid, backgroundPIDs);
                printf("Running Background PID: %i\n", spawnpid); // announce running bg process
            }                                    
			break;
	}
                                              
}

/*
    redirectIO

    :parameter: cmdStruct - pointer to command line struct containing user entered commands

    This function sets up any IO redirection based on the arguments pased in by the user
    for background processes it redirects to /dev/null. This was done to avoid confilicting 
    output text from background processes.
*/
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
            fflush(stdout);
            exit(1); // exit with status 1
        }
        else
        {
             dup2(fileDesc[0], 0); // redirect stdin to the given input file
        }       
    }  

    // check for output file
    if(cmdStruct->outputFile != NULL)
    {
        fileDesc[1] = open(cmdStruct->outputFile, O_CREAT | O_WRONLY | O_APPEND | O_CLOEXEC, 0700); // open output file for read only set to close by exec functions
        
        // check for error
        if(fileDesc[1] == -1)
        {
            perror("Open input file error: "); // opening file failed
            fflush(stdout);
            exit(1); // exit with status 1
        }
        else
        {
            dup2(fileDesc[1], 1); // redirect stdout to the given output file
        }       
    }
    // redirect stdout to /dev/null to avoid catastrophic errors
    else if(cmdStruct->outputFile == NULL && cmdStruct->isBackground == true)
    {           
        fileDesc[1] = open("/dev/null", O_WRONLY | O_CLOEXEC); // open /dev/null for writing
        dup2(fileDesc[1], 1); // redirect stdout to the given /dev/null
    }

}

/*
*   checkExitStatus
*
*   :parameter: lastFGStat - this is the exit status of the last executed foreground process
*
*   This function provides the functionality for the status command. It will check the last exit status 
*   of a foreground function and print it out to the user.
*/
void checkExitStatus(int lastFGStat)
{
    // check for a normal exit status
    if(WIFEXITED(lastFGStat) != 0)
    {
        printf("foreground process exited with status: %i\n", WEXITSTATUS(lastFGStat));
        fflush(stdout);
    }

    // check for signal termination
    if(WIFSIGNALED(lastFGStat) != 0)
    {
        printf("foreground process terminated by signal: %i\n", WTERMSIG(lastFGStat));
        fflush(stdout);
    }
}


/*
*    checkBackground
*
*   :parameter: backgroundPIDs[] - this is an array used to keep track of background PIDs
*
 *   This function will check the array of background PIDs and
 *   if one has exited, it will invoke the function to print a 
*    message to the user that the background process has finished
*/
void checkBackground(int backgroundPids[])
{   
    int finishedPID, exitStatus; // will hold the process ID and exit status of finished background process

    for(int i = 0; i < BGPROCS; i++)
    {   
        // check all non zero values for finished status
        if(backgroundPids[i] != 0 && (finishedPID = waitpid(backgroundPids[i], &exitStatus, WNOHANG)) > 0)
        {
            // check for a normal exit status
            if(WIFEXITED(exitStatus) != 0)
            {
                printf("Background process %i exited with status: %i\n", finishedPID,WEXITSTATUS(exitStatus));
                fflush(stdout);
                removeBGPID(finishedPID, backgroundPids); // remove from tracking array
            }

            // check for signal termination
            if(WIFSIGNALED(exitStatus) != 0)
            {
                printf("Background process %i terminated by signal: %i\n", finishedPID,WTERMSIG(exitStatus));
                fflush(stdout);
                removeBGPID(finishedPID, backgroundPids); // remove from tracking array
            }
        }
    }
}

/*
    trackBGPID

    :parameter: pid - a process id of a background process
    :parameter: backgroundPIDs[] - an array containing background process IDs

    This function will add a new background process PID to the first empty 
    array index it finds in backgroundPIDs[]
*/
void trackBGPID(int pid, int backgroundPIDs[])
{
    // search for the first 0 value spot update the value to the provided pid
    for(int i = 0; i < BGPROCS; i++)
    {
        if(backgroundPIDs[i] == 0)
        {
            backgroundPIDs[i] = pid; // update to new PID value
            break;
        }
    }
}

/*
    removeBGPID

    :parameter: pid - a process id of a background process
    :parameter: backgroundPIDs[] - an array containing background process IDs

    This function will reset the index in backgroundPIDs array that matches pid
    back to 0 and make it available to store another pid
*/
void removeBGPID(int pid, int backgroundPIDs[])
{
    // search for and remove matching pid
    for(int i = 0; i < BGPROCS; i++)
    {
        if(backgroundPIDs[i] == pid)
        {
            backgroundPIDs[i] = 0; // reset to 0
            break;
        }
    }
}

/*
*   SIGTSTP_Handler
*
*   :parameter: signo - this is the terminating signal number 
*
*   This is the custom signal handler for SIGTSTP signal (ctrl - z)
*
*   Source Cited:
*   Title: Exploration: Signal Handling API
*   Author: Unknown OSU Instructor
*   URL: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
*   Description: I heavily referenced this exploration module and used a modified version of the 
*   code for my own application of custom signal handlers.*
*/
void SIGTSTP_Handler(int signo)
{   
    // toggle foreground mode
    if(foreground_only == false)
    {
        foreground_only = true;
        char *testTxt = "\nGoing into foreground mode. Background Processes (&) Now Ignored.\n";
        write(STDOUT_FILENO, testTxt, strlen(testTxt));
    }
    else
    {
        foreground_only = false;
        char *testTxt = "\nExiting foreground mode. Background Processes (&) Now Allowed.\n";
        write(STDOUT_FILENO, testTxt, strlen(testTxt));
    }   
}