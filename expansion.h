/*
Joseph Polaski
CS344: Program 3

expansion.h

This headerfile contains all prototypes for functions related to 
expansion of "$$" string for smallsh. Function definitions in
expansion.c

Sources Cited:
Title: C Header Files and Prototypes
Author: Mark Kevin Baltazar
URL: https://medium.com/@markkevinbaltazar/c-header-files-and-prototypes-170ecdf526d
Description: I read this article in order to learn how to properly implement header files for 
function prototyping.
*/

// header guard
#ifndef EXPANSION_
#define EXPANSION_

char *expand$$(char *tokenString); // checks for expansion and processes it
int canExpand(char *tokenString); // performs check for expansion
int checkFormat(char *tokenString, int startIndex); // checks format of token string
char *performExpansion(char *tokenString, int startIndex); // performs string expansion for "$$"

#endif