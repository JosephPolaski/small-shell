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
void printStruct(struct userCommands *testStruct);

#endif