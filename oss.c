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

static void createChildren(Options opts, int bufferSize);
static pid_t createChild(int childIndex, int numberToTest, int bufferSize);

//static void waitForChildren(Clock * clockPtr);
void cleanUpAndExit(int param);
static void cleanUp();
static void addSignalHandlers();

const static Clock CLOCK_INCREMENT = {0, 100000}; // Virtual time increment
const static Clock MAX_TIME = {2, 0};		  // Time limit for children
const static char * CHILD_PATH = "./child";	  // Path to child executable

static char * shm; // Pointer to the shared memory region

int main(int argc, char * argv[]){
	Options opts;	// Options struct defined in ossOptions.h
	int bufferSize; // The number of bytes in the shared memory region
	exeName = argv[0]; // Assigns to global defined in perrorExit.c

	// Sets alarm and signal handling
	alarm(2);
	addSignalHandlers();

	// Parses options, printing help and exiting on -h
	opts = getOptions(argc, argv);

	/* DEBUG */
        printf("n - %d\nnStr - %s\ns - %d\nsStr - %s\nb - %d\nbStr - %s\ni - %d\niStr - %s\no - %s\n",
                opts.numChildrenTotal,
                opts.numChildrenTotalStr,
                opts.simultaneousChildren,
                opts.simultaneousChildrenStr,
                opts.beginningIntTested,
                opts.beginningIntTestedStr,
                opts.increment,
                opts.incrementStr,
                opts.outputFileName
        );

	// Creates and initializes shared memory region
	bufferSize = sizeof(Clock) + opts.numChildrenTotal * sizeof(int);
	shm = (char *) sharedMemory(bufferSize, IPC_CREAT);
	initializeSharedMemory(shm, bufferSize, '\0');

	// Forks and execs child
	createChildren(opts, bufferSize);

	pid_t pid = wait(NULL);	
	// Waits for children to finish executing
//	waitForChildren((Clock *) shm);
	
	// Detatches from and removes shared memory segment
	cleanUp();	

	return 0;
}

static void addSignalHandlers(){
	struct sigaction sigact;

	sigact.sa_handler = cleanUpAndExit;
	sigact.sa_flags = 0;

	if ((sigemptyset(&sigact.sa_mask) == -1) ||
	    (sigaction(SIGALRM, &sigact, NULL)) == -1 ||
	    (sigaction(SIGINT, &sigact, NULL))	== -1)
		perrorExit("Faild to install SIGALARM signal handler");
}

void cleanUpAndExit(int param){
	cleanUp();
	perrorExit("Terminating after recieving a signal");
}

// Creates children up to the specified limits
static void createChildren(Options opts, int bufferSize){
	createChild(2, 3, bufferSize);
}

// Forks and execs child with params as command line args, returns pid
static pid_t createChild(int childIndex, int numberToTest, int bufferSize){
	pid_t pid;// Child pid

	// Forks
	if ((pid = fork()) == -1) perrorExit("createChild faild to fork");

	// Execs if child, returns pid of child if parent
	if (pid == 0){
		// Converts integer args to strings
		char index[100];
		sprintf(index, "%d", childIndex);
	
		char toTest[100];
		sprintf(toTest, "%d", numberToTest);

		char buff[100];
		sprintf(buff, "%d", bufferSize);

		// Execs child
		execl(CHILD_PATH, CHILD_PATH, index, toTest, buff, NULL);
		perrorExit("createChild - exec failed");
	}
	
	// Returns pid of child to parent
	return pid;
	
}

/*
static void waitForChildren(Clock * clock){
	pid_t child;
	int childIndex = 0;

	while ((child = waitpid(-1, &childIndex, WNOHANG)) >= 0 ){

		// Increments and prints clock
		incrementClock(clock, CLOCK_INCREMENT);
		printf("oss - Seconds: %d Nanoseconds: %d\n",
			clock->seconds,
			clock->nanoseconds
		);
		fflush(stdout);
		
		// Breaks if there are no children or non-interrupt error
		if ((child == -1) && (errno != EINTR)) break;

		// Kills children if time limit reached
		if (clockCompare(clock, &MAX_TIME) >= 0)
			 cleanUp((char*)clock);
	}
}
*/

static void cleanUp(){
	// Detatches from and removes shared memory. Defined in sharedMemory.c
	detach(shm);
	removeSegment(); // shmid is in static variable in sharedMemory.c
	
	// Kills all processes in the same process group
	signal(SIGQUIT, SIG_IGN);
	kill(0, SIGQUIT);
}
