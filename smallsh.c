/*
Joseph Polaski
CS344: Program 3

smallsh.c

This is the main source file for the smallsh shell program. 

*/

#define _GNU_SOURCE // access nonstandard GNU/Linux functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
        userCmdLine = getUserCommandLine(MAXCMDLEN);

        buildCmdStruct(userCmdLine);

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
        if(token == "<")
        {
            token = strtok(NULL, " "); // skip to input file name
            cmdStruct->inputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string
            strcpy(cmdStruct->inputFile, token); // copy string to inputFile data member
        }
        // check for output file
        else if(token == ">")
        {
            token = strtok(NULL, " "); // skip to output file name
            cmdStruct->outputFile = calloc(strlen(token) + 1, sizeof(char)); // dynamically allocate memory for token size string
            strcpy(cmdStruct->outputFile, token); // copy string to outputFile data member
        }
        // check for background flag
        else if(token == "&")
        {
            cmdStruct->isBackground = true; // set background flag to true
        }
        // treat as command or argument
        else
        {
            // check token for expansion
            // add to args array
            // iterate counter
            expand$$(token);
        }

        token = strtok(NULL, " "); // retreive next token
    }
    
}