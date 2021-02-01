/*
Joseph Polaski
CS344: Program 3

smallsh.h

This headerfile contains all prototypes for functions defined in smallsh.c

*/

// header guard
#ifndef SMALLSH_
#define SMALLSH_

char *getUserCommandLine(int maxLength); // prompts user and recieves command line
struct userCommands *buildCmdStruct(char *userCmdLine); // build a struct out of the user entered command line
void printStruct(struct userCommands *testStruct); // TEST PURPOSES ONLY
int runUserCommands(struct userCommands *cmdStruct, int *lastProcStat); // runs the commands entered by the user
void executeOthers(struct userCommands *cmdStruct, int *lastProcStat); // this function executes all non custom bash commands given
void redirectIO(struct userCommands *cmdStruct); // this function handles any file redirection necessary based on user inputs
void checkExitStatus(int lastFGStat); // checks and prints out the exit status of the last foreground process

#endif