/*
Joseph Polaski
CS344: Program 3

expansion.c

This source file contains function definitions related to the expansion of 
the "$$" string for smallsh.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "expansion.h"

/*
    expand$$

    This function will check for the "$$" string. If detected 
    it will process the tokenString input parameter to replace
    "$$" with the process ID.
*/
char *expand$$(char *tokenString)
{   
    int startIndex = -1; // starting index of first instance of "$$". This is inialized to -1 because 0 is a valid index.
    int endIndex = 0; // will hold the index value of the portion of the string that is after "$$" in cases where it is sandwiched

    startIndex = canExpand(tokenString); // check for expansion and retrieve index of "$$"" if found
    checkFormat(tokenString);

    //"$$" is present and must be expanded to be process ID
    if(startIndex != -1)
    {


    }
}

/*
    canExpand

    This function will check if expansion is possible. If so it will
    return the index of the first occurance of "$$"
*/
int canExpand(char *tokenString)
{
    char *check;
    check = strstr(tokenString, "$$"); // check for first instance of "$$"

    // if an instance of "$$" exists in tokenString
    if(check)
    {
        // calculate and return index of 1st instance of "$$"
        int subStrLocation = check - tokenString; // obtain index of check by calculating the difference from the beginning pointer (0)     
        return subStrLocation;
    }
    else
    {          
        return -1; // no instance of "$$" found
    }
}

/*
    checkFormat

    :parameter: tokenString - the string entered by the user containing "$$"
    :returns: 1, 2, 3 or 4 representing the format types outlined below.
    
    This function will determine the format of the "$$" containing string. I chose to use regex for this 
    because it seemed like a good way to search for string patterns and I have always wanted to learn more about it
    so this gave me the opportunity. The purpose of this function is to help the performExpansion function know how
    to process the string containing "$$".

    let s be some arbitrary string, this function will check for the 3 following formats:
    1. atBegin - "$$" is at the beginning of the string formatted as $$s
    2. sandwich - "$$" is between 2 strings or charcters formatted as s$$s
    3. atEnd - "$$" is at the end of a string formatted as s$$
    4. is$$ - the complete string is "$$" by itself


*/
int checkFormat(char *tokenString)
{
    // check for format 4 (is$$)
    if(strcmp(tokenString, "$$") == 0)
    {
        return 4;
    }

    // check other potential formats
    // declare regex variables
    regex_t atBegin, sandwich, atEnd;
    int resultF1 = -1, resultF2 = -1, resultF3 = -1; // will hold the results of the regex comparisons 0 will mean a match
    int test;

    // compile regex patterns
    test = regcomp(&atBegin,"^\\${2}([a-zA-Z0-9_\\.\\s]+)" , 0); // format #1: "$$" at the beginning of a string
    printf("compile result: %i\n", test);
    regcomp(&sandwich,"([a-zA-Z0-9_\\. ]+)\\${2}([a-zA-Z0-9_\\.\\$ ]+)" , 0); // format #2: "$$" in the middle of a string
    regcomp(&atEnd,"(([a-zA-Z0-9_\\.\\$ ]+)\\${2}" , 0); // format #3: "$$" at end of a string

    // perform regex comparisons to determine format
    resultF1 = regexec(&atBegin, tokenString, 0, NULL, 0); // check for format #1
    resultF2 = regexec(&sandwich, tokenString, 0, NULL, 0); // check for format #2
    resultF3 = regexec(&atEnd, tokenString, 0, NULL, 0); // check for format #3

    if(resultF1 == 0)
    {
        printf("$$ at beginning of string\n");
    }
    else if(resultF2 == 0)
    {
        printf("$$ is sandwiched\n");
    }
    else if(resultF3 == 0)
    {
        printf("$$ is at the end!\n");
    }

}

