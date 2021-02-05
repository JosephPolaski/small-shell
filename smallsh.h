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
int runUserCommands(struct userCommands *cmdStruct, int *lastProcStat, struct sigaction sigIntAction, int backgroundPIDs[]); // runs the commands entered by the user
void executeOthers(struct userCommands *cmdStruct, int *lastProcStat, struct sigaction sigIntAction, int backgroundPIDs[]); // this function executes all non custom bash commands given
void redirectIO(struct userCommands *cmdStruct); // this function handles any file redirection necessary based on user inputs
void checkExitStatus(int lastFGStat); // checks and prints out the exit status of the last foreground process
void trackBGPID(int pid, int backgroundPIDs[]); // will add a new background pid to the tracking list
void removeBGPID(int pid, int backgroundPIDs[]); // removes a matching pid from the tracking list
void checkBackground(int backgroundPids[]);
void SIGINT_Handler(int signum);
void SIGSTP_Handler(int signo);

#endif