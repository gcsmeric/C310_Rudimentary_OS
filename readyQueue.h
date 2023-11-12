#ifndef READY_QUEUE
#define READY_QUEUE
typedef struct ReadyQueue {
    PCB* head;
    PCB* tail;
    int length;
} ReadyQueue;

void init (ReadyQueue* q);
int isEmpty (ReadyQueue* q);
void enqueue (ReadyQueue* q, PCB* pcb, char* policy);
void printQueue (ReadyQueue* q);
int RRChecker (ReadyQueue* q);
void ageProcesses (ReadyQueue* q);
void reorganizeQueue (ReadyQueue* q, PCB* promotedPCB);
void SchedulerRun (ReadyQueue* q, char* policy);
void getHead (ReadyQueue* q);
void getTail (ReadyQueue* q);
void put_to_back (ReadyQueue* q, PCB* pcb);
void replace_frame (ReadyQueue* q, int LRU_CACHE[]);
//void fill_in_frame_store (ReadyQueue* q, int start_index);
void fill_in_frame_store (ReadyQueue* q, int start_index, int LRU_CACHE[]);
#endif // READY_QUEUE