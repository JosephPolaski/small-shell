**Small Shell (Smallsh)**
-------------------------
**A lightweight custom shell written in C**

-------------------------
This is a project that I completed for my Operating Systems I class. It is a lightweight shell designed for Linux. This documentation will give an overview of the shell and how to compile and run it.

**Overview**
---
-------
This project was developed with Visual Studio Code on CentOS Linux. The unique features of Smallsh are as follows:

* Supports 3 custom commands for cd, exit and status.
* all other commands with arguments fork a new process and are executed via execvp().
* IO redirection is suppported via '<' and '>' symbols in the user command line.
*  Execution of background processes is supported and can be invoked with '&' character as the ending command line argument.
* Custom signal handlers for SIGINT and SIGTSTP
    - **SIGINT (ctrl + c):** can be used to kill foreground child processes. Ignored by parent shell.
    - **SIGTSTP (ctrl + z):** toggles foreground only mode. In this mode any background processes asttempted will be run in the foreground.
* Variable expansion of '$$': anywhere the user enters '$$' whether by itself or part of a string, this shell will recognize it as the process ID of the shell.
* Note: the pipe operator '|' is not supported by smallsh

**Compile and Run**
---
-------
compile from the command line using

```
gcc --std=c99 -g -o smallsh smallsh.c expansion.c
```
run and explore!

```
./smallsh
```
