// oss.c was created by Mark Renard on 2/21/2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "ossOptions.h"
#include "sharedMemory.h"
#include "clock.h"
#include "perrorExit.h"


/* Prototypes */

static void addSignalHandlers();
static void cleanUpAndExit(int param);
static void cleanUp();
static void createChildren(Options opts, int bufferSize);
static pid_t createChild(int childIndex, Options opts, int bufferSize);
static pid_t * pidArray(int numPids);
static int childIndex(pid_t childPid, pid_t * pidArray, int size);
static void printCreatedInfo(FILE * fp, int childIndex, pid_t childPid);
static void printCompletedInfo(FILE * fp, int childIndex, pid_t childPid);
static void printResults(FILE * fp, Options opts);


/* Named Constants */

const static Clock CLOCK_INCREMENT = {0, 10000};  // Virtual time increment
const static char * CHILD_PATH = "./child";	  // Path to child executable
const static int BUFF_SZ = 100;			  // Size of character buffer
const static int MAX_EXECUTION_SECONDS = 2;	  // Max total execution time

/* Static Global Variables */

static char * shm; 	   	// Pointer to the shared memory region
static FILE * fp = NULL;  	// Output file handler
static const char * filePath;	// Path to the output file

int main(int argc, char * argv[]){
	Options opts;	   // Options struct defined in ossOptions.h
	int bufferSize;    // The number of bytes in the shared memory region

	exeName = argv[0]; // Assigns to global defined in perrorExit.c

	// Sets alarm and signal handling
	alarm(MAX_EXECUTION_SECONDS);
	addSignalHandlers();

	// Parses options, printing help and exiting on -h
	opts = getOptions(argc, argv);
	
	// Stores the name of the output file
	filePath = opts.outputFileName;

	// Creates and initializes shared memory region
	bufferSize = sizeof(Clock) + opts.numChildrenTotal * sizeof(int);
	shm = (char *) sharedMemory(bufferSize, IPC_CREAT);
	initializeSharedMemory(shm, bufferSize, '\0');

	// Forks and execs child
	createChildren(opts, bufferSize);
	
	// Prints clock, detatches from and removes shared memory segment
	cleanUp();	

	return 0;
}

// Registers cleanUpAndExit as handler for signals SIGALRM and SIGINT
static void addSignalHandlers(){
	struct sigaction sigact;

	// Initializes sigaction values
	sigact.sa_handler = cleanUpAndExit;
	sigact.sa_flags = 0;

	// Assigns signals to sigact
	if ((sigemptyset(&sigact.sa_mask) == -1) ||
	    (sigaction(SIGALRM, &sigact, NULL)) == -1 ||
	    (sigaction(SIGINT, &sigact, NULL))	== -1)
		perrorExit("Faild to install signal handler");
}

// Signal handler that deallocates shared memory, terminates children, and exits
void cleanUpAndExit(int param){
	cleanUp();

	// Prints error message
	char buff[BUFF_SZ];
	sprintf(buff,
		 "%s: Error: Terminating after receiving a signal",
		 exeName
	);
	perror(buff);

	exit(1);
}

// Prints clock, detatches from and removes shared memory, and terminates children
static void cleanUp(){

	// Prints curent simulated time to output file
	if (fp == NULL && (fp = fopen(filePath, "w")) == NULL){

		// Prints error message if the file couldn't be open
		char buff[BUFF_SZ];
		sprintf(buff, "%s: Error: Couldn't open file", exeName);
		perror(buff);
	} else {
		// Prints the time
		fprintf(fp, "Process terminated at ");
		printTime(fp, (Clock *)shm);
		fclose(fp);
	}

	// Detatches from and removes shared memory. Defined in sharedMemory.c
	detach(shm);
	removeSegment(); // shmid is in static variable in sharedMemory.c
	
	// Kills all other processes in the same process group
	signal(SIGQUIT, SIG_IGN);
	kill(0, SIGQUIT);
}

// Creates children up to the specified limits
static void createChildren(Options opts, int shmSize){

	// Assigns options to local variables for readability
	int numTotal = opts.numChildrenTotal;
	int simultaneous = opts.simultaneousChildren;
	
	int numCreated = 0;	// Number of children created so far
	int numFinished = 0;	// Number of children that finished executing

	pid_t returnVal;	// Stores the return value of waitpid
	pid_t * childPids;	// Array of pids of all created child processes

	if ((fp = fopen(opts.outputFileName, "w+")) == NULL)
		perrorExit("Couldn't open file"); 

	// Initializes empty array of pids
	childPids = pidArray(numTotal);

	// Creates initial simultaneous child processes
	int initial = (simultaneous < numTotal) ? simultaneous : numTotal;
	int i;
	for(i = 0; i < initial; i++){
		childPids[i] = createChild(i, opts, shmSize);
		printCreatedInfo(fp, i, childPids[i]);
	}
	numCreated = initial;

	// Launches new processes when old ones finish until numTotal reached
	while (numFinished < numTotal){
		incrementClock((Clock *)shm, CLOCK_INCREMENT);

		// Gets pid of finished child if status is immediately available
		returnVal = waitpid(-1, NULL, WNOHANG);

		// Checks for unrecoverable errors and exits
		if ((returnVal == -1) && (errno != EINTR)){
			perrorExit("createChildren - waitpid error");
		}
		
		// If a child finished, prints info and replaces if necessary
		if (returnVal != 0){
			
			// Prints info on the finished child
			int index = childIndex(returnVal, childPids, numTotal);
			printCompletedInfo(fp, index, returnVal);
			numFinished++;

			// Creates a replacement child if necessary
			if (numCreated < numTotal){
				pid_t pid; // Temporary storage

				pid = createChild(numCreated, opts, shmSize);
				childPids[numCreated] = pid;
				
				//Prints info on newly created child
				printCreatedInfo(fp, numCreated, pid);

				numCreated++;
			}
		}
	}

	printResults(fp, opts);
}

// Forks and execs child with params as command line args, returns pid
static pid_t createChild(int childIndex, Options opts, int shmSize){
	pid_t pid; 		// Child pid
	int testNum;	// The number the child will test for primality

	// Forks
	if ((pid = fork()) == -1) perrorExit("createChild faild to fork");

	// Execs if child, returns pid of child if parent
	if (pid == 0){
		// Computes number to test
		testNum = opts.beginningIntTested + childIndex * opts.increment;

		// Converts integer args to strings
		char index[BUFF_SZ];
		sprintf(index, "%d", childIndex);
	
		char toTest[BUFF_SZ];
		sprintf(toTest, "%d", testNum);

		char shmSz[BUFF_SZ];
		sprintf(shmSz, "%d", shmSize);

		// Execs child
		execl(CHILD_PATH, CHILD_PATH, index, toTest, shmSz, NULL);
		perrorExit("createChild - exec failed");
	}
	
	// Returns pid of child to parent
	return pid;	
}

// Returns a pointer to empty allocated memory for numPids pids
static pid_t * pidArray(int numPids){
	pid_t * arr = malloc(numPids * sizeof(pid_t));

	// Initializes pids to 0	
	int i;
	for (i = 0; i < numPids; i++){
		arr[i] = 0;
	}

	return arr;
}

// Returns the logical identifier of a child process stored in a pid array
static int childIndex(pid_t childPid, pid_t * pidArray, int size){
	int i;
	for (i = 0; i < size; i++){
		if (pidArray[i] == childPid){
			pidArray[i] = 0; // Zeros pid in case it's re-used
			return i;
		}
	}

	// Exits if no child is found
	char buf[BUFF_SZ];
	sprintf(buf, "Couldn't find index of child with pid %d", (int)childPid);
	perrorExit(buf);

	return -1;
}

// Prints a child's index, pid, and current simulated time as creation time
static void printCreatedInfo(FILE * fp, int childIndex, pid_t childPid){
	fprintf(fp, "%03d : %09d - Child %d with pid %d created! \n\n",
		((Clock *)shm)->seconds,
		((Clock *)shm)->nanoseconds,
		childIndex,
		(int)childPid
	);
	
}

// Prints info on the results of a child process that has terminated
static void printCompletedInfo(FILE * fp, int childIndex, pid_t childPid){
	
	// Stores the location of the result in shared memory
	int * result = (int *)(shm + sizeof(Clock) + childIndex * sizeof(int));

	// Prints the index, pid, and termination time of the process
	fprintf(fp, "%03d : %09d - Child %d with pid %d terminated.\n",
		((Clock *)shm)->seconds,
		((Clock *)shm)->nanoseconds,
		childIndex,
		(int)childPid
	);

	// Prints the results of computation
	if (*result == -1){
		fprintf(fp, "Time limit exceeded, no result obtained.\n\n");
	} else if (*result < 0){
		fprintf(fp, "It found that %d is NOT prime.\n\n", -*result);
	} else {
		fprintf(fp, "It found that %d is prime!\n\n", *result);
	}
	fflush(stdout);
}

// Prints lists of numbers with primality prime, not prime, and not determined
static void printResults(FILE * fp, Options opts){
	int * array;	// Pointer to shared array in shared memory
	int i;		// Index variable
	int n;		// The number that was tested

	// Gets shared array from shared memory
	array = (int *)(shm + sizeof(Clock));

	// Prints prime numbers
	fprintf(fp, "\nThe following were found to be prime:\n");
	for (i = 0; i < opts.numChildrenTotal; i++)
		if (array[i] > 0) fprintf(fp, "%d ", array[i]);

	// Prints non-primes
	fprintf(fp, "\n\nThe following were found NOT to be prime:\n");
	for (i = 0; i < opts.numChildrenTotal; i++)
		if (array[i] < -1) fprintf(fp, "%d ", -array[i]);

	// Prints numbers the primality of which was not determined
	fprintf(fp, "\n\nThe primality of the following was not determined "
		"due to time constraints:\n"
	);
	for (i = 0; i < opts.numChildrenTotal; i++){
		if (array[i] == -1){

			// Computes the tested integer for 
			n = opts.beginningIntTested + i * opts.increment;
			fprintf(fp, "%d ", n);
		}
	}
	fprintf(fp, "\n\n");

}
