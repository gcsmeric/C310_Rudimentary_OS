#include <stdio.h>
#include <stdlib.h>

typedef struct PCB {
    int pID; // program ID
    int startInt; //start memory location
    int PC; // program counter
    int length; //script length
    char* name; // script name
    struct PCB* next; //next PCB in queue
    int job_length_score; // job length score used for AGING
    int pagetable[100]; // page table that keeps track 
    int current_frame; // tells us which frame we're currently executing code from 
    int current_frame_index; // tells us where in the current frame we're executing code from 
} PCB;

// function that creates a new PCB based on the inputs provided and returns the PCB
PCB* createPCB(int pID, int startInt, int PC, int length, char* name, int pagetable[], int current_frame, int current_frame_index) {
    PCB *new = (PCB *) malloc(sizeof(PCB));
    new->pID=pID;
    new->startInt=startInt;
    new->PC=PC;
    new->length=length;
    new->job_length_score = length;
    new->name=name;
    new->next=NULL;
    new->pagetable[0] = pagetable[0];
    new->pagetable[1] = pagetable[1];
    for (int i=2; i< 100; i++)
    {
        new->pagetable[i] = -10000;
    }
    new->current_frame = current_frame;
    new->current_frame_index = current_frame_index;
    return new;
}

// function that checks if a process has finished executing
int isDone (PCB* pcb) {
    if (pcb->PC >= pcb->length) {
        return 1;
    }
    return 0;
}