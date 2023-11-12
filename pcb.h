#ifndef __PCB_H_
#define __PCB_H_
typedef struct PCB {
    int pID; // program ID
    int startInt; //start memory location
    int PC; // program counter
    int length; //script length
    char* name; // script name
    struct PCB* next; //next PCB in queue
    int job_length_score;
    int pagetable[100];
    int current_frame; 
    int current_frame_index;
} PCB;

PCB* createPCB(int pID, int startInt, int PC, int length, char* name, int pagetable[], int current_frame, int current_frame_index);
int runFromMemIntervalNew(PCB* pcb, int LRU_CACHE[]);
int isDone (PCB* pcb);
#endif // __PCB_H_