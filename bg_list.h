/*
Joseph Polaski
CS344: Program 3

bg_list.h

This headerfile contains all prototypes for functions defined in bg_list.c
These support the doubly linked list I implemented to maintain background
process PIDs.

*/

// header guard
#ifndef BG_LIST_
#define BG_LIST_

struct bg_node *createBGList(void); // initializes a new empty doubly linked list for background processes
void trackBgPID(struct bg_node *headNode, int bgPID); // inserts ('Tracks') a new PID in the list
void insertNode(struct bg_node *current, struct bg_node *newNode); // performs insertion of a node to the spot after current
void deleteNode(struct bg_node *current); // deletes the current node from the list
void removeBgPID(struct bg_node *headNode, int bgPID); // searches for a matching PID and removes the corresponding node
void printList(struct bg_node *headNode); // for testing purposes prints linked list

#endif