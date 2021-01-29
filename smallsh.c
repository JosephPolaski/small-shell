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
#include "smallsh.h"

int main(void)
{   
    const int MAXCMDLEN = 2048; // maximum characters allowed in command line
    const int MAXARGS = 512; // maximum arguments allowed in command line

    char *userCmdLine; // will hold the command line from the user max length 2048 characters

    // Establish Main Loop Flag
    enum progStatus {active, inactive};
    enum progStatus shellStatus = active;

    // initiate main loop
    while(shellStatus == active)
    {   
        userCmdLine = getUserCommandLine(MAXCMDLEN);

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
    fgets(cmdLine, sizeof(cmdLine), stdin);
    cmdLine[strlen(cmdLine)-1] = '\0'; // strip off ending \n from fgets

    return (cmdLine);
}