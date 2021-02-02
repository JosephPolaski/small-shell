/*
Joseph Polaski
CS344: Program 3

bg_list.c

This source file contains the functions and struct definition for a doubly linked list I chose
to implement as a way to track my background process PIDs. I chose to implement a doubly linked
list because it would allow for easy insertion and deletion of PIDs from the list without 
having to worry about resizing and re-ordering a fragmented array.

*/
#include "bg_list.h"
#include <stdio.h>
#include <stdlib.h>

struct bg_node 
{
    int bg_pid; // will hold the PID of a background process
    struct bg_node *prev; // previous node pointer
    struct bg_node *next; // next node pointer
};

/*
    createBGList

    This function will run at the beginning of smallsh 
    it will initialize an empty doubly linked list and 
    return the head pointer.
*/
struct bg_node *createBGList(void)
{
    // declare new sentinel nodes and allocate heap memory for them
    struct bg_node *head = malloc(sizeof(struct bg_node));
    struct bg_node *tail= malloc(sizeof(struct bg_node));

    // set head pointers and data
    head->prev = NULL;
    head->next = tail;
    head->bg_pid = -1; // arbitrary negative value as all PIDs are positive

    // set tail pointers and data
    tail->prev = head;
    tail->next = NULL;
    tail->bg_pid = -1; // arbitrary negative value as all PIDs are positive

    return head;
};

/*
    trackBgPID

    This function will track a background PID by 
    creating a new node to represent it and inserting it into the 
    bgProcList in smallsh.
*/
void trackBgPID(struct bg_node *headNode, int bgPID)
{   
    // create new node
    struct bg_node *newNode = malloc(sizeof(struct bg_node));    

    newNode->bg_pid = bgPID; // set to PID parameter for background process    

    // check for empty list case (head is pointing to tail)
    if(headNode->next->bg_pid == -1)
    {   
        // insert new node
        insertNode(headNode, newNode);
    }
    // list is not empty
    else
    {
        struct bg_node *current = headNode; // set iteration pointer

        // iterate to end of linked list and append new node
        while(current != NULL)
        {
            // if the last node before tail is found insert
            if(current->next->next == NULL)
            {
                // insert new node
                insertNode(current, newNode);
            }

            current = current->next; // iterate pointer
        }
    }
}

/*
    removeBgPID

    This function will track a background PID by 
    creating a new node to represent it and inserting it into the 
    bgProcList in smallsh.
*/
void removeBgPID(struct bg_node *headNode, int bgPID)
{
    struct bg_node *current = headNode; // set iteration pointer

    // iterate to end of linked list and search for a matching PID
    while(current != NULL)
    {
        // if a matching PID is found
        if(current->bg_pid == bgPID)
        {
            // delete node with matching PID
            deleteNode(current);
        }
        current = current->next; // iterate list pointer
    }
}

/*
    insertNode

    This function will insert a new node after current
*/
void insertNode(struct bg_node *current, struct bg_node *newNode)
{
    newNode->next = current->next; // new node next points to current next pointer
    newNode->prev = current; // new node previous pointer points to current node

    current->next->prev = newNode; // previous pointer of current-> next points to new node
    current->next = newNode; // current node next pointer points to new  node
}

/*
    deletNode

    This function will delete the current node
*/
void deleteNode(struct bg_node *current)
{
    struct bg_node *garbage = current; // capture a pointer to current for freeing

    // re arrange pointers of neighboring nodes
    current->prev->next = current->next;
    current->next->prev = current->prev;
    free(garbage); // delete node and free memory
}

// prints the linked list for a visual for testing purposes
void printList(struct bg_node *headNode)
{
    struct bg_node *current = headNode; // set iteration pointer

    // iterate to end of linked list and search for a matching PID
    while(current != NULL)
    {
       printf("%i -> ", current->bg_pid);

       current = current->next;
    }
    printf("\n");
}
