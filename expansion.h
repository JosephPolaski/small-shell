/*
Joseph Polaski
CS344: Program 3

expansion.h

This headerfile contains all prototypes for functions related to 
expansion of "$$" string for smallsh. Function definitions in
expansion.c

*/

// header guard
#ifndef EXPANSION_
#define EXPANSION_

char *expand$$(char *tokenString); // checks for expansion and processes it
int canExpand(char *tokenString); // performs check for expansion
int checkFormat(char *tokenString); // checks format of token string


#endif