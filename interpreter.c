//exec tests/test1.txt tests/test2.txt tests/test3.txt FCFS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"
#include "readyQueue.h"

int MAX_ARGS_SIZE = 7;
// pID value
int pIDval = 1;
int *pIDvalPointer = &pIDval;

int help();
int quit();
int badcommand();
int badcommandSameFilesExec();
int badcommandTooManyTokens();
int badcommandFileDoesNotExist();
int set(char *var, char *value);
int print(char *var);
PCB* loadToMemGeneratePCB(char *script);
PCB* loadToFrameStoreGeneratePCB(char *script, int number_of_frames);
int runFromMemInterval(int startInt, int PC, int numLines, int programLength, char *script);
int getFileSize (char* name);
void rearrange (int number_of_processes);
int run(char *script, ReadyQueue* q);
int my_ls();
int echo();

int interpreter(char *command_args[], int args_size)
{
	int i;

	if (args_size < 1 || args_size > MAX_ARGS_SIZE)
	{
		if (strcmp(command_args[0], "set") == 0 && args_size > MAX_ARGS_SIZE)
		{
			return badcommandTooManyTokens();
		}
		return badcommand();
	}

	for (i = 0; i < args_size; i++)
	{ // strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}


	// if the command we're looking at is either an exec or run then copy the files into the backstore
	if (strcmp (command_args[0], "exec") == 0 || strcmp (command_args[0], "run") == 0) 
	{
		for (int i=1; i< args_size - 1; i++) 
		{
			// creating string that is "cp filename backstore" which will be given to system as input
			char cp_command[100] = "cp ";
			strcat (cp_command, command_args[i]);
			strcat (cp_command, " backstore");

			// copy file into backstore
			system (cp_command);
		}
	}

	if (strcmp(command_args[0], "help") == 0)
	{
		// help
		if (args_size != 1)
			return badcommand();
		return help();
	}
	else if (strcmp(command_args[0], "quit") == 0)
	{
		// quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}
	else if (strcmp(command_args[0], "set") == 0)
	{
		// set
		if (args_size < 3)
			return badcommand();
		char *value = (char *)calloc(1, 150);
		char spaceChar = ' ';

		for (int i = 2; i < args_size; i++)
		{
			strncat(value, command_args[i], 30);
			if (i < args_size - 1)
			{
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	}
	else if (strcmp(command_args[0], "exec") == 0)
	{
		// if the number of args is incorrect
		if (args_size > 5 || args_size < 2)
		{
			return badcommand();
		}

		// if the last input is a valid scheduling policy
		if (strcmp(command_args[args_size - 1], "FCFS") == 0 || strcmp(command_args[args_size - 1], "SJF") == 0 || strcmp(command_args[args_size - 1], "RR") == 0 || strcmp(command_args[args_size - 1], "AGING") == 0)
		{
			// allocate space for the queue
			ReadyQueue* q = malloc (sizeof(ReadyQueue));
			// initialize the ready queue
			init (q);

			// executing a single process 
			if (args_size == 3)
			{
				return run(command_args[1], q);
			}
			// executing multiple processes 
			else
			{	
				// create PCB's and add them to the ready queue
				for (int i = 1; i<args_size-1; i++) {

					// get the number of lines in the file 
					// we will use the number of lines to determine how many frames the file should take when initially being loaded into frame store
					int number_of_lines = getFileSize (command_args[i]);

					// if the file has 3 or less lines then only one page is loaded into frame store
					if (number_of_lines <= 3) {
						// load to frame store taking 1 frame and get PCB
						PCB* pcb = loadToFrameStoreGeneratePCB(command_args[i], 1);

						// add PCB to the readyQueue "q"
						enqueue (q, pcb, command_args[args_size - 1]);
					}
					// if the file has more than 3 lines then two pages are loaded into frame store
					else {
						// load to frame store taking 2 frames and get PCB
						PCB* pcb = loadToFrameStoreGeneratePCB(command_args[i], 2);

						// add PCB to the readyQueue "q"
						enqueue (q, pcb, command_args[args_size - 1]);
					}
				}

				// resets the none2 lines we set when we called loadToFrameStoreGeneratePCB back to none
				removeNone2 ();
				
				// execute ready queue based on policy 
				if (strcmp(command_args[args_size - 1], "FCFS") == 0) {
					SchedulerRun (q, "FCFS");
				}
				// WE WILL ONLY CONSIDER THIS POLICY AS BEING VALID FOR A3
				else if (strcmp(command_args[args_size - 1], "RR") == 0) {
					// run the queue with the RR policy 
					SchedulerRun (q, "RR");
					// reset frame store 
					resetframestore ();
				}
				else if (strcmp(command_args[args_size - 1], "SJF") == 0) {
					SchedulerRun (q, "SJF");
				}
				else if (strcmp(command_args[args_size - 1], "AGING") == 0) {
					SchedulerRun (q, "AGING");
				}

				mem_clear_interval(0, 1000);

				return 0;
			}
		}

		// if the last input is NOT a scheduling policy print an error message
		return badcommand();
	}

	else if (strcmp(command_args[0], "print") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}
	else if (strcmp(command_args[0], "run") == 0)
	{
		if (args_size != 2)
			return badcommand();
		
		ReadyQueue* q = malloc (sizeof(ReadyQueue));
		// initialize the ready queue
		init (q);

		return run(command_args[1], q);
	}
	else if (strcmp(command_args[0], "my_ls") == 0)
	{
		if (args_size > 2)
			return badcommand();
		return my_ls();
	}
	else if (strcmp(command_args[0], "echo") == 0)
	{
		if (args_size > 2)
			return badcommand();
		return echo(command_args[1]);
	}
	else
		return badcommand();
}

int help()
{

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit()
{
	printf("%s\n", "Bye!");

	// deleting backstore
	char delete_backstore_command[50] = "rm -r backstore";
	system (delete_backstore_command);

	exit(0);
}

int badcommand()
{
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandSameFilesExec()
{
	printf("%s\n", "Bad command: same file name");
	return 1;
}

int badcommandTooManyTokens()
{
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int badcommandFileDoesNotExist()
{
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int set(char *var, char *value)
{

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;
}

int print(char *var)
{
	printf("%s\n", mem_get_value(var));
	return 0;
}

// function that loads processes into memory, creates their PCB, and returns it
// NOT USED IN A3 
PCB* loadToMemGeneratePCB(char *script) {
	int counter = 0;
	// where file starts in memory
	int startInt;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	// if file is not valid
	if (p == NULL)
	{
		badcommandFileDoesNotExist();
	}

	fgets(line, 999, p);
	while (1)
	{
		// code that generates the key for a process 
		// an example of a key is filenameprog1line1
		char key[1000];
		strcpy(key, "filename");
		strcat(key, script);
		strcat(key, "line");
		char linenumber[20];
		sprintf(linenumber, "%d", counter);
		strcat(key, linenumber);

		// if line number is 0 
		if (counter == 0)
		{
			//get startInt 
			startInt = mem_set_value(key, line);
		}
		else
		{
			mem_set_value(key, line);
		}
		memset(line, 0, sizeof(line));

		if (feof(p))
		{
			break;
		}
		counter++;
		fgets(line, 999, p);
	}

	// create PCB using the information we have gathered about the process
	PCB* createdPCB = createPCB((*pIDvalPointer), startInt, 0, counter + 1, script, NULL, startInt, 0);
	// increment pid so that we ensure unique PIDs
	(*pIDvalPointer)++;
	// return PCB to be added to ready queue
	return createdPCB;
}

// function that takes a file name as input and outputs the number of lines in that file
int getFileSize (char* name) {
	FILE* fp = fopen (name, "r");
	char c;
	int count = 1;

	do {
		c = fgetc (fp);
		if (c == '\n') count++;
	} while (c != EOF);

	fclose (fp);
	return count;
}

// function that loads processes into frame store, creates their PCB, and returns it
PCB* loadToFrameStoreGeneratePCB(char *script, int number_of_frames) {
	int counter = 0;
	// where file starts in memory
	int startInt;
	char line[1000];
	int pagetable[2];
	FILE *p = fopen(script, "rt"); // the program is in a file

	// if file is not valid
	if (p == NULL)
	{
		badcommandFileDoesNotExist();
	}

	int lines_added = 0;

	fgets(line, 999, p);
	while (1)
	{
		if (lines_added == (number_of_frames * 3)) {
			break;
		}

		// code that generates the key for a process 
		// an example of a key is filenameprog1line1
		char key[1000];
		strcpy(key, "filename");
		strcat(key, script);
		strcat(key, "line");
		char linenumber[20];
		sprintf(linenumber, "%d", counter);
		strcat(key, linenumber);

		// if line number is 0 
		if (counter == 0)
		{
			//get startInt 
			startInt = frame_store_set_value (key, line);

			// set the first element of the pagetable to the starting index of the file in the frame store
			pagetable[0] = startInt;
			lines_added++;
		}
		else if (counter == 3) 
		{
			// set the second element of the pagetable to the starting index of the second frame that the file takes in the frame store
			pagetable[1] = frame_store_set_value (key, line);
			lines_added++;
		}
		else
		{
			frame_store_set_value (key, line);
			lines_added++;
		}

		memset(line, 0, sizeof(line));

		// if we reach the end of the file
		if (feof(p))
		{
			// code that sets the empty space in the frames that have at least one line from our file to none2
			// prevents adding code from the next file into a frame that already has code from this file 
			// so if we have a frame with one line the frame will be:
			// echo P1_L1
			// none2
			// none2
			// so when the next file runs frame_store_set_value (key, line) it wont add code into the none2 lines because they aren't none
			// note that after we load all the files into the frame store we then call removeNone2 () which sets all these none2's to nones
			for (int i=0; i< (3 - (lines_added % 3)) % 3; i++) {
				frame_store_set_value ("none2", "none2");
			}
			break;
		}
		counter++;
		fgets(line, 999, p);
	}

	// create PCB using the information we have gathered about the process
	// note that we set the current_frame and current_frame_index both to 0 because we are looking at the first 
	// frame that the file takes and the first line within that frame
	PCB* createdPCB = createPCB((*pIDvalPointer), startInt, 0, getFileSize (script), script, pagetable, 0, 0);

	// increment pid so that we ensure unique PIDs
	(*pIDvalPointer)++;

	// return PCB to be added to ready queue
	return createdPCB;
}

//function that runs code in given interval
// Chose to create this new run function so that the inputs to the original run function remained simple
int runFromMemInterval(int startInt, int PC, int numLines, int programLength, char *script) {
	// if program has completed its execution there's nothing to run so return
	if (PC >= programLength) return 0;
	int endpoint = PC + numLines;
	if (programLength < endpoint) {
		endpoint = programLength;
	}

	// for loop that goes through all the lines that need to be executed and executes them
	for (int i = PC; i < endpoint; i++)
	{
		char key[1000];
		strcpy(key, "filename");
		strcat(key, script);
		strcat(key, "line");
		char linenumber[20];
		sprintf(linenumber, "%d", i);
		strcat(key, linenumber);
		char *line = frame_store_get_value(key);

		// if we are trying to execute a line that is NOT in the frame store
		if (strcmp (line, "Variable does not exist") == 0) {
			// if there is an empty frame in the frame store
			// if (empty_space_check () != -1) {
			// 	printf ("filling in page...\n");
			// 	return -1;
			// }
			// // if there is NOT an empty frame in the frame store
			// else {
			// 	printf ("need to check LRU page...\n");
			// 	return -2;
			// }
			printf ("GOING TO EMPTY SPACE CHECK\n");
			return empty_space_check ();
		}
		parseInput(line);
	}
}

// function that runs up to 2 lines of code from pcb 
int runFromMemIntervalNew(PCB* pcb, int LRU_CACHE[]) {
	// for loop that loops up to two times and executes a line from the file at each iteration 
	for (int i=0; i< 2; i++)
	{
		// if the program has completed its execution there's nothing to run so return 
		if (pcb -> PC >= pcb -> length) {
			return 0;
		}

		// if we are trying to execute a line that is NOT in the frame store we encounter a page fault
		if (pcb -> pagetable[pcb -> current_frame] == -10000) 
		{
			// call empty_space_check () which tells us if we have an empty frame in the frame store to add code into 
			// empty_space_check () returns -1 if we DO have an empty frame and -2 if we DONT have an empty frame 
			return empty_space_check (); 
		}

		// frame store index to execute 
		// eg. if we want to access the fourth line of the first program which is in frame 2 index 0 
		// we get from the page table the index of the second frame and we add 0 to it 
		// so 3 + 0 = 3 so we run the line at index 3 in the frame store
		int frame_store_index_to_execute = ( pcb -> pagetable[pcb -> current_frame] ) + ( pcb -> current_frame_index );

		// get the line needed to be executed using the index above
		char* line = frame_store_get_value_at_index( frame_store_index_to_execute );

		// update LRU cache ages due to cache content being accessed 
		for (int i=0; i<get_frame_store_length () / 3; i++) {
			if (i == frame_store_index_to_execute / 3) {
				LRU_CACHE[i] = 0;
			} 
			else {
				LRU_CACHE[i]++;
			}
		}

		// run the line
		parseInput(line);

		// if we've executed the last line from the frame go to the next frame and set the current_frame_index to 0 
		if ( pcb -> current_frame_index == 2 ) {
			pcb -> current_frame++ ;
			pcb -> current_frame_index = 0;
		}
		// if we still haven't executed all the lines from the frame to go the next line from frame
		else {
			pcb -> current_frame_index++;
		}

		// update the PC of the file so we know when we finish executing the entire file
		pcb -> PC++;
	}
	
}

int run(char *script, ReadyQueue* q)
{

	// creating string that is "cp filename backstore" which will be given to system as input
	char cp_command[100] = "cp ";
	strcat (cp_command, script);
	strcat (cp_command, " backstore");

	// copy file into backstore
	system (cp_command);

	// get number of lines of the file to know how many frames to fill with code
	int number_of_lines = getFileSize (script);

	PCB* pcb;

	// if the file has 3 or less lines then only one page is loaded into frame store
	if (number_of_lines <= 3) {
		// load code into frame store taking 1 frame 
		pcb = loadToFrameStoreGeneratePCB(script, 1);
		enqueue (q, pcb, "RR");
	}
	// if the file has more than 3 lines then two pages are loaded into frame store
	else {
		// load code into frame store taking 2 frames
		pcb = loadToFrameStoreGeneratePCB(script, 2);
		enqueue (q, pcb, "RR");
	}

	// reset none2's to none's
	removeNone2 ();

	// get frame size to set up LRU 
    int frame_store_size = get_frame_store_length () / 3;

    // LRU CACHE
    int LRU_CACHE[frame_store_size];
    for (int i=0; i<frame_store_size; i++) {
        LRU_CACHE[i] = 0;
    }

	while (pcb->PC <= pcb->length - 1)
	{
		// run code and update LRU cache 
		// runFromMemIntervalNew will output -1 if there is at least one empty frame in the frame store and -2 if there are no empty frames
		int output = runFromMemIntervalNew (q->head, LRU_CACHE);

		// if there is a page fault and there is at least one empty frame in the frame store
		if ( output == -1 ) {
			// get the starting index of this empty frame 
			int empty_spot = getFirstEmptyLine ();

			// fill in frame store at this index 
			fill_in_frame_store (q, empty_spot, LRU_CACHE);
		}
		// if there is a page fault and there is NOT at least one empty frame in the frame store
		else if ( output == -2 ) {
			// replace frame based on LRU frame 
			replace_frame (q, LRU_CACHE);
		} 
	}

	// reset frame store
	resetframestore ();

	return 0;
}

int exec()
{
	int errCode;
	return errCode;
}

int my_ls()
{
	int errCode = system("ls | sort");
	return errCode;
}

int echo(char *var)
{
	if (var[0] == '$')
	{
		var++;
		printf("%s\n", mem_get_value(var));
	}
	else
	{
		printf("%s\n", var);
	}
	return 0;
}