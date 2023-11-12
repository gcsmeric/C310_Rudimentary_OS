#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "pcb.h"
#include "readyQueue.h"

#define SHELL_MEM_LENGTH 1000


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
struct memory_struct frame_store[FRAME_STORE_SIZE];
struct memory_struct variable_store[VARIABLE_STORE_SIZE];

// Shell memory functions

void mem_init(){

	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}

	// initialize frame store
	for (i=0; i<FRAME_STORE_SIZE; i++){		
		frame_store[i].var = "none";
		frame_store[i].value = "none";
	}

	// initialize variable store
	for (i=0; i<VARIABLE_STORE_SIZE; i++){		
		variable_store[i].var = "none";
		variable_store[i].value = "none";
	}
}

int get_frame_store_length () {
	return FRAME_STORE_SIZE;
}

void print_sizes () {
	printf ("Frame Store Size = %d; Variable Store Size = %d\n", FRAME_STORE_SIZE, VARIABLE_STORE_SIZE);
}

void print_frame_store () {
	printf ("PRINTING FRAME STORE\n");
	for (int i=0; i< FRAME_STORE_SIZE; i++)
	{
		printf ("line %d var = %s and value = %s\n", i, frame_store[i].var, frame_store[i].value);
	}
	printf ("DONE\n");
}

void print_variable_store () {
	printf ("PRINTING VARIABLE STORE\n");
	for (int i=0; i< VARIABLE_STORE_SIZE; i++)
	{
		printf ("var = %s and value = %s\n", variable_store[i].var, variable_store[i].value);
	}
	printf ("DONE\n");
}

// function that clears memory after a process is ran
void mem_clear_interval(int start, int end){

	int i;
	for (i=start; i<end+1; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// function that resets variable store
void resetmem () {
	for (int i=0; i< VARIABLE_STORE_SIZE; i++)
	{
		variable_store[i].var = "none";
		variable_store[i].value = "none";
	}
}

void resetframestore () {
	for (int i=0; i< FRAME_STORE_SIZE; i++)
	{
		frame_store[i].var = "none";
		frame_store[i].value = "none";
	}
}

// function that prints the first lines of memory for debugging purposes
void printMemory () {
	for (int i=0; i< 150; i++)
	{
		printf("at index %d, var is %s, value is %s\n", i, shellmemory[i].var, shellmemory[i].value);
	}
}

// Set key value pair
int mem_set_value(char *var_in, char *value_in) {
	int i;

	// if value already exists 
	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variable_store[i].var, var_in) == 0){
			variable_store[i].value = strdup(value_in);
			return i;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variable_store[i].var, "none") == 0){
			variable_store[i].var = strdup(var_in);
			variable_store[i].value = strdup(value_in);
			return i;
		} 
	}

	return -1;

}

// function that adds line of the file into the frame store 
int frame_store_set_value(char *var_in, char *value_in) {
	int i;

	for (i=0; i<FRAME_STORE_SIZE; i++){
		if (strcmp(frame_store[i].var, "none") == 0) {
			frame_store[i].var = strdup(var_in);
			frame_store[i].value = strdup(value_in);
			return i;
		} 
	}

	return -1;

}

void frame_store_set_value_at_index(int index, char* var, char* value) {
	frame_store[index].var = strdup(var);
	frame_store[index].value = strdup(value);
}

// function that resets none2 values to none in the frame store 
void removeNone2 () {
	for (int i=0; i< FRAME_STORE_SIZE; i++)
	{
		if (strcmp (frame_store[i].var, "none2") == 0 && strcmp (frame_store[i].value, "none2") == 0) {
			frame_store[i].var = "none";
			frame_store[i].value = "none";
		}
	}
}

// function that tells us if we have an empty frame to add code into or not 
// so tells us how to handle page fault 
// either add code into empty space in frame store or replace a frame using LRU 
int empty_space_check () {
	// for every frame in the frame store
	for (int i=0; i< FRAME_STORE_SIZE; i=i+3)
	{
		// for every line in the frame at index i
		for (int j=i; j< i + 3; j++)
		{
			// if we found an empty space 
			if (strcmp (frame_store[j].var, "none") == 0 && strcmp (frame_store[j].value, "none") == 0) {
				// if we reached the last line in the frame
				if (j == i + 2) {
					// return -1 which we will use in the SchedulerRun function to indicate that we must fill in code into the frame
					return -1;
				}
			}
			// if we encounter a line that isn't empty we break and look at the next frame 
			else {
				break;
			}
		}
	} 

	// if we don't have an empty frame to add code into we return -2 which we will use in the SchedulerRun function to indicate that we must replace a frame
	return -2;
}

// function that outputs the index of the first line in a frame that is empty 
int getFirstEmptyLine () {
	// for every frame 
	for (int i=0; i< FRAME_STORE_SIZE; i=i+3)
	{
		// for every line in the frame at index i
		for (int j=i; j< i + 3; j++)
		{
			// if we found an empty space 
			if (strcmp (frame_store[j].var, "none") == 0 && strcmp (frame_store[j].value, "none") == 0) {
				if (j == i + 2) {
					return i;
				}
			}
			else {
				break;
			}
		}
	} 
	return -1;
}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variable_store[i].var, var_in) == 0){
			return strdup(variable_store[i].value);
		} 
	}
	return "Variable does not exist";
}

//get value based on input key
char *frame_store_get_value(char *var_in) {
	int i;

	for (i=0; i<FRAME_STORE_SIZE; i++){
		if (strcmp(frame_store[i].var, var_in) == 0){
			return strdup(frame_store[i].value);
		} 
	}
	return "Variable does not exist";
}

// get the line at index i from the frame store
char *frame_store_get_value_at_index (int i) {
	char* line = (frame_store[i]).value;

	if (strcmp (line, "none") == 0) {
		return "Variable does not exist";
	}

	return line;
}

// function that fills in lines from the file into the frame starting at start_index
void fill_in_frame_store (ReadyQueue* q, int start_index, int LRU_CACHE[]) {
	int count = -1;
	char line[1000];
	int tmp_start_index = start_index;

	FILE *p = fopen(q -> head -> name, "rt"); // the program is in a file

	// reach the line(s) we want to add to the frame store
	for (int i=0; i<= q -> head -> PC; i++)
	{
		fgets(line, 999, p);
		count++;
	}


	// update page table to reflect the addition of this new frame
	for (int i=0; i< 100; i++) 
	{
		if (q -> head -> pagetable[i] == -10000) {
			q -> head -> pagetable[i] = (start_index);
			break;
		}
	}


	// add code from the file into this new frame
	for (int i=0; i< 3; i++)
	{
		char key[1000];
		strcpy(key, "filename");
		strcat(key, q -> head -> name);
		strcat(key, "line");
		char linenumber[20];
		sprintf(linenumber, "%d", count);
		strcat(key, linenumber);		

		// add code into tmp_start_index
		frame_store_set_value_at_index (tmp_start_index, key, line);

		if (feof (p)) {
			break;
		}

		fgets(line, 999, p);
		count++;

		// increment tmp_start_index
		tmp_start_index++;
	}

	//update LRU cache ages due to cache content being accessed
	for (int i=0; i<get_frame_store_length () / 3; i++) {
		if (i == start_index / 3) {
			LRU_CACHE[i] = 0;
		} 
		else {
			LRU_CACHE[i]++;
		}
	}
}

// function that replaces frame using LRU policy
void replace_frame (ReadyQueue* q, int LRU_CACHE[]) {

	//find frame to replace
	int max_age = 0;
	for (int i=0; i<get_frame_store_length () / 3; i++) {
		if (LRU_CACHE[i] > max_age) max_age = LRU_CACHE[i];
	}

	int frame_to_replace = 0;
	for (int i=0; i<get_frame_store_length () / 3; i++) {
		if (LRU_CACHE[i] == max_age) {
			frame_to_replace = i;
			break;
		}
	}

	// print contents of frame
	printf ("Page fault! Victim page contents:\n");
	for (int i=0; i< 3; i++)
	{
		char* line = frame_store_get_value_at_index (( frame_to_replace * 3 ) + i);
		// remove the \n's 
		strtok(line, "\n");
		printf ("%s\n", line);
	}
	printf("End of victim page contents.\n");

	// remove contents from frame store
	for (int i=0; i< 3; i++)
	{
		frame_store_set_value_at_index (( frame_to_replace * 3 ) + i, "none", "none");
	}

	// fill in code into LRU frame 
	fill_in_frame_store (q, frame_to_replace * 3, LRU_CACHE);
}