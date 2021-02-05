/*
Joseph Polaski
CS344: Program 3

expansion.c

This source file contains function definitions related to the expansion of 
the "$$" string for smallsh. All instances of "$$" will be recognized as the
process ID of Smallsh.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "expansion.h"

/*
    expand$$

    :parameter: tokenString - current token string being evaluated
    :returns: expanded form of token string if possible, otherwise returns original string

    This function will check for the "$$" string. If detected 
    it will process the tokenString input parameter to replace
    "$$" with the process ID.
*/
char *expand$$(char *tokenString)
{   
    int startIndex = -1; // starting index of first instance of "$$". This is inialized to -1 because 0 is a valid index.
    startIndex = canExpand(tokenString); // check for expansion and retrieve index of "$$"" if found


    //"$$" is present and must be expanded to be process ID
    if(startIndex != -1)
    {
        // perform "$$" expansion and capture in new buffer
        char *expandedString;
        expandedString = performExpansion(tokenString, startIndex);

        return expandedString;       
    }
    else
    {   
        // can't be expanded return original string
        return tokenString;
    }
}

/*
    canExpand

    :parameter: tokenString - current token string being evaluated

    This function will check if expansion is possible. If so it will
    return the index of the first occurance of "$$"
*/
int canExpand(char *tokenString)
{
    char *check;
    check = strstr(tokenString, "$$"); // check for first instance of "$$"

    // if an instance of "$$" exists in tokenString
    if(check != NULL)
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
    performExpansion

    :parameter: tokenString - current token string being evaluated
    :parameter: startIndex - starting index of first instance of "$$" in string

    This function will replace the first instance of $$ 
    in a string with the process ID. 
*/
char *performExpansion(char *tokenString, int startIndex)
{
    char *expandedString = calloc(2048, sizeof(char)); // dynamicall allocate adequate memory amount for unknown length
    int k = 0; // represents index in expandedString

    // begin copying characters from token string to expanded string
    for(int i = 0; i < strlen(tokenString); i++)
    {   
        // check for an instance of "$$"
        if(tokenString[i] == '$' && tokenString[i+1] == '$')
        {
            int processID = getpid(); // get PID of smallsh
            char tempPidStr[12];    // temp string to hold string version of pid
            memset(tempPidStr, '\0', 12); // set buffer to all '\0'
            sprintf(tempPidStr,"%i", processID); // convert PID integer to string and store in tempPidStr
            
            // Copy all PID characters to expanded string
            for(int j = 0; j < strlen(tempPidStr); j++)
            {
                expandedString[k] = tempPidStr[j];
                k++; // increment last insertion point for expandedString
            }
            i++; // increment i to skip over second '$' char in "$$"
                 // This is to avoid inserting process id 2 times in cases of "$$$"
        }
        else
        {
            // no instance of "$$"" copy character from token string to expanded string as normal
            expandedString[k] = tokenString[i];
            k++; // increment last insertion point for expandedString
        }
    }
    expandedString[strlen(expandedString)] = '\0'; // append null terminator to created string
    return expandedString;
}

