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
static void createChild(Options opts, int bufferSize);
static void waitForChildren(Clock * clockPtr);
static void cleanUp(char * shm);

const static Clock CLOCK_INCREMENT = {0, 100000}; // Virtual time increment
const static Clock MAX_TIME = {2, 0};		 // Time limit for children

int main(int argc, char * argv[]){
	Options opts;	// Options struct defined in ossOptions.h
	char * shm;	// Pointer to first byte of shared memory region
	int bufferSize; // The number of bytes in the shared memory region

	exeName = argv[0]; // Assigns to global defined in perrorExit.c

	// Parses options, printing help and exiting on -h
	opts = getOptions(argc, argv);

	/* DEBUG */
	printf("n - %d\ns - %d\nb - %d\ni = %d\no - %s\n",
                opts.numChildrenTotal,
                opts.simultaneousChildren,
                opts.beginningIntTested,
                opts.increment,
                opts.outputFileName
        );

	// Creates and initializes shared memory region
	bufferSize = sizeof(Clock) + opts.numChildrenTotal * sizeof(int);
	shm = (char *) sharedMemory(bufferSize, IPC_CREAT);
	initializeSharedMemory(shm, bufferSize, '\0');

	// Forks and execs child
	createChildren(opts, bufferSize);
	
	// Waits for children to finish executing
	waitForChildren((Clock *) shm);
	
	// Detatches from and removes shared memory segment
	cleanUp(shm);	

	return 0;
}

// Creates children up to the specified limits
static void createChildren(Options opts, int bufferSize){
	createChild(opts, bufferSize);
}

static void createChild(Options opts, int bufferSize){
	pid_t pid;	// Child pid

	// Forks
	if ((pid = fork()) == -1) perrorExit("createChild faild to fork");

	// Tests exec
	char buff[100];
	sprintf(buff,"%d", bufferSize);
	if (pid == 0){
		execl("./child", "child", "1", "2", buff, NULL);
		perrorExit("createChild - exec failed");
	}

}

static void waitForChildren(Clock * clock){
	pid_t child;
	int childIndex = 0;

	while ((child = waitpid(-1, &childIndex, WNOHANG)) >= 0 ){

/*		// Increments and prints clock
		incrementClock(clock, CLOCK_INCREMENT);
		printf("oss - Seconds: %d Nanoseconds: %d\n",
			clock->seconds,
			clock->nanoseconds
		);
		fflush(stdout);
*/		
		// Breaks if there are no children or non-interrupt error
		if ((child == -1) && (errno != EINTR)) break;

		// Kills children if time limit reached
		if (clockCompare(clock, &MAX_TIME) >= 0)
			 cleanUp((char*)clock);
	}

	// Print child index
	printf("Child index: %d\n", childIndex);
	fflush(stdout);
}

static void cleanUp(char * shm){
	// Detatches from and removes shared memory. Defined in sharedMemory.c
	detach(shm);
	removeSegment(); // shmid is in static variable in sharedMemory.c
	
	// Kills all processes in the same process group
	signal(SIGQUIT, SIG_IGN);
	kill(0, SIGQUIT);
}
