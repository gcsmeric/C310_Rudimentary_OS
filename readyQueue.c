#include <stdio.h>
#include <string.h>
#include "pcb.h"
#include "interpreter.h"
#include "shellmemory.h"
#include <stdlib.h>

typedef struct ReadyQueue {
    PCB* head; // head of the queue
    PCB* tail; // tail of the queue
    int length; // length of the queue
    int LRU_cache[]; // LRU cache
} ReadyQueue;

// function that initializes the queue
void init (ReadyQueue* q) {
    q->length = 0;
    q->head = NULL;
    q->tail = NULL;
}

// functions that get the head and the tail of the queue 
void getHead (ReadyQueue* q);
void getTail (ReadyQueue* q);
void replace_frame (ReadyQueue* q, int LRU_CACHE[]);

// function that checks if queue is empty
int isEmpty (ReadyQueue* q) {
    return q->tail == NULL;
}

// function that prints the queue for debugging purposes
void printQueue (ReadyQueue* q) {
    PCB* iter = q->head;

    for (int i=0; i< q->length; i++)
    {
        printf("PRINTING... PCB is %s length is %d, pc is %d, startInt is %d, job score is %d, and pid is %d pagetable[0] is %d and pagetable[1] is %d pagetable[2] is %d current_frame is %d and current_frame_index is %d\n", iter->name, iter->length, iter->PC, iter->startInt, iter->job_length_score, iter->pID, iter->pagetable[0], iter->pagetable[1], iter->pagetable[2], iter->current_frame, iter->current_frame_index);
        iter = iter->next;
    }
    getHead (q);
    getTail (q);
}

void getHead (ReadyQueue* q) {
    printf("Head is %s\n", q->head->name);
}

void getTail (ReadyQueue* q) {
    printf("Tail is %s\n", q->tail->name);
}

// puts pcb to the back of the readyQueue
void put_to_back (ReadyQueue* q, PCB* pcb) {
    // printf ("In put to back with %s -> %s -> %s\n", q -> head -> name, q -> head -> next -> name, q -> head -> next -> next -> name);
    PCB* tail = q -> head;
        while (tail -> next != NULL)
        {
            tail = tail -> next;
        }

    if (pcb == q->head) {
        // get the second process which is going to be the new head
        PCB* second = q -> head -> next;

        // set the old process's next to NULL (signifying that its at the back of the ready queue)
        q -> head -> next = NULL;

        q -> head = second;
        PCB* iter = q -> head;
        while (iter -> next != NULL)
        {
            iter = iter -> next;
        }

        iter -> next = pcb;

        // printf ("New queue is :\n");
        // printQueue (q);
    }

    else if (pcb == tail) {
        return;
    }
    
    else {
        PCB* prevnode = q->head;
        while (!(prevnode->next == pcb)) {
            prevnode = prevnode->next;
        }
        PCB* nextnode = pcb->next;
        prevnode->next = nextnode;
        pcb->next = NULL;
        PCB* iter = q->head;
        while (iter -> next != NULL)
        {
            iter = iter -> next;
        }

        iter -> next = pcb;
    }
}

// function that adds a new PCB to the queue based on a policy
void enqueue (ReadyQueue* q, PCB* pcb, char* policy) {
    if (isEmpty (q)) {
        q->head = q->tail = pcb;
        q->length++;
        return;
    }
    else {
        if (strcmp(policy, "FCFS") == 0 || strcmp(policy, "RR") == 0) {
            q->tail->next = pcb;
            q->tail = pcb;
            q->length++;
        }
        // if SJF add to queue in ascending order of length 
        else if (strcmp(policy, "SJF") == 0) {
            PCB* cur = q->head;
            PCB* prev = NULL;

            while (cur != NULL && cur->length <= pcb->length) {
                prev = cur;
                cur = cur->next;
            }
            pcb->next = cur;
            if (prev != NULL) {
                prev->next = pcb;
            }
            else {
                q->head = pcb;
            }
            q->length++;
        }
        // if AGING add to queue in ascending order of job length score 
        else if (strcmp(policy, "AGING") == 0) {
            PCB* cur = q->head;
            PCB* prev = NULL;

            while (cur != NULL && cur->job_length_score <= pcb->job_length_score) {
                prev = cur;
                cur = cur->next;
            }
            pcb->next = cur;
            if (prev != NULL) {
                prev->next = pcb;
            }
            else {
                q->head = pcb;
            }
            q->length++;
        }
    }
    // setting tail 
    PCB* iter = q->head;
    while (iter->next != NULL) {
        iter = iter->next;
    }
    q->tail = iter;
}

//checks to see if there is any file that has not been fully executed
int RRChecker (ReadyQueue* q) {
    PCB* iter = q->head;

    for (int i=0; i< q->length; i++)
    {
        if (iter->PC <= iter->length - 1) {
            return 0;
        }
        iter = iter->next;
    }
    return 1;
}

// function that ages processes
void ageProcesses (ReadyQueue* q) {
    PCB* iter = q->head;
    //skip the head because we don't age it
    iter = iter->next;

    for (int i=1; i< q->length; i++)
    {
        if (iter->job_length_score == 0) {
            iter = iter->next;
            continue;
        }
        iter->job_length_score = iter->job_length_score - 1;
        iter = iter->next;
    }
}

// function that returns the process that needs to be promoted
// if no process needs to be promoted it returns the head
PCB* promotionCheck (ReadyQueue* q) {
    PCB* iter = q->head;
    int head_job_score = iter->job_length_score;
    // if process is done 
    if (isDone (iter)) {
        iter->job_length_score = 9999999;
    }

    iter = iter->next;

    for (int i=1; i< q->length; i++)
    {
        if (iter->job_length_score < head_job_score) {
            // return iter so we know where in the queue the PCB that needs to be promoted is 
            return iter;
        }
    }
    // there is no PCB that has a job length score less than the head so return the head
    return q->head;
}

// function that reorganizes the queue if there is a PCB that needs promoting
void reorganizeQueue (ReadyQueue* q, PCB* promotedPCB) {
    // if there are 2 PCB's in the queue 
    if (q->length == 2) {
        PCB* oldHead = q->head;
        q->head = promotedPCB;
        q->head->next = oldHead;
        q->tail = oldHead;
    }
    // if there are 3 PCB's in the queue
    else if (q->length == 3) {
        PCB* first = q->head;
        PCB* second = q->head->next;
        PCB* third = q->head->next->next;

        int index = 0;
        // checking to see which PCB in the queue is the one that needs to be promoted
        if (second->pID == promotedPCB->pID) {
            index = 2;
        }
        else if (third->pID == promotedPCB->pID) {
            index = 3;
        }

        // process to be promoted is the second one is the queue 
        if (index == 2) {
            // rearrange queue accordingly
            q->head = promotedPCB;
            q->head->next = third;
            q->head->next->next = first;
            q->tail = first;
        }
        // process to be promoted is the third one is the queue 
        else if (index == 3) {
            // rearrange queue accordingly
            q->head = promotedPCB;
            q->head->next = second;
            q->head->next->next = first;
            q->tail = first;
        }
    }
}

// function that runs the queue based on a given policy
void SchedulerRun (ReadyQueue* q, char* policy) {

    // get the number of frames in the frame store by dividing the frame store length by 3
    int frame_store_size = get_frame_store_length () / 3;

    // LRU CACHE which has length equal to the number of frames in the frame store
    int LRU_CACHE[frame_store_size];

    // initialize the LRU cache's elements all to 0 
    for (int i=0; i<frame_store_size; i++) {
        LRU_CACHE[i] = 0;
    }

    // execute processes if policy is FCFS
    if (strcmp (policy, "FCFS") == 0) {
        PCB* iter = q->head;
        for (int i=0; i< q->length; i++)
        {
            runFromMemInterval (iter->startInt, iter->PC, iter->length, iter->length, iter->name);
            iter = iter->next;
        }
    }
    // execute processes if policy is RR
    else if (strcmp (policy, "RR") == 0) {

        // while we still have some files that have not been fully executed
        while (RRChecker (q) != 1 && q->head != NULL) {
            
            // call runFromMemIntervalNew which runs the code from the head of the ready queue outputs a value that tells us how to handle a page fault
            int output = runFromMemIntervalNew (q->head, LRU_CACHE);

            // if there is a page fault and there is at least one empty frame in the frame store
            if ( output == -1 ) {
                // get the starting index of the frame
                int empty_spot = getFirstEmptyLine ();

                // fill in the frame store with the lines needed to be loaded
                fill_in_frame_store (q, empty_spot, LRU_CACHE);
            }
            // if there is a page fault and there is NOT at least one empty frame in the frame store
            else if ( output == -2 ) {
                // replace frame based on values in LRU cache
                replace_frame (q, LRU_CACHE);
            } 
            // put process to back of the ready queue
            put_to_back (q, q->head);
        }
    }

    // execute processes if policy is SJF
    else if (strcmp (policy, "SJF") == 0) {
        // since the queue is already sorted in ascending order of length just execute the processes starting from the head
        PCB* iter = q->head;
        for (int i=0; i< q->length; i++)
        {
            runFromMemInterval (iter->startInt, iter->PC, iter->length, iter->length, iter->name);
            iter = iter->next;
        }
    }

    // execute processes if policy is AGING
    else if (strcmp (policy, "AGING") == 0) {
        PCB* iter = q->head;
        int counter = 0;

        // while we still have some files that have not been fully executed
        while (RRChecker (q) != 1) {
            // if process is not done run one line from it
            if (!isDone (iter)) {
                runFromMemInterval (iter->startInt, iter->PC, 1, iter->length, iter->name);
                iter->PC = iter->PC + 1;
            }

            // age processes after running one line from the head of the queue
            ageProcesses (q);

            // get the PCB that needs to be promoted
            PCB* pcbToBePromoted = promotionCheck (q);

            // if no process has been promoted continue executing the head of the queue
            if (pcbToBePromoted == q->head) {
                continue;
            }

            // if there has been a promotion reorganize the queue
            reorganizeQueue (q, pcbToBePromoted);

            // set the iter to the newly promoted PCB at the head
            iter = q->head;
        }
    }
}