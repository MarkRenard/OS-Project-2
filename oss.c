// oss.c was created by Mark Renard on 2/21/2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#include "ossOptions.h"
#include "sharedMemory.h"
#include "clock.h"
#include "perrorExit.h"

static Clock * initializeClock(char * shm);
static void createChildren(Options opts, int bufferSize);
static void createChild(Options opts, int bufferSize);
static void waitForChildren(Clock * clockPtr);

const static int CLOCK_INCREMENT = 10000;

int main(int argc, char * argv[]){
	Options opts;	// Options struct defined in ossOptions.h
	char * shm;	// Pointer to first byte of shared memory region
	int bufferSize; // The number of bytes in the shared memory region

	exeName = argv[0]; // Assigns to global defined in perrorExit.c

	// Parses options, printing help and exiting on -h
	opts = getOptions(argc, argv);

	// Creates shared memory region
	bufferSize = sizeof(Clock) + opts.numChildrenTotal * sizeof(int);
	shm = (char *) sharedMemory(bufferSize, IPC_CREAT);

	// Sets clock to 0
	Clock * clockPtr = initializeClock(shm);

	// Forks and execs child
	createChildren(opts, bufferSize);

	// Waits for children to finish executing
	waitForChildren(clockPtr);
	
	// Prints value of clock;
	printf("oss - Seconds: %d Nanoseconds: %d\n",
		clockPtr->seconds,
		clockPtr->nanoseconds
	);	

	// Detatches from and removes shared memory segment
	detach(shm);
	removeSegment();	

	return 0;
}

// Initializes clock in shared memory region shm to 0 seconds 0 nanoseconds
static Clock * initializeClock(char * shm){
	Clock * clockPtr = (Clock *) shm;
	clockPtr->seconds = 0;
	clockPtr->nanoseconds = 0;

	return clockPtr;
}

static void createChildren(Options opts, int bufferSize){
	createChild(opts, bufferSize);
}

static void createChild(Options opts, int bufferSize){
	pid_t pid;	// Child pid

	// Forks
	if ((pid = fork()) == -1) perrorExit("createChild faild to fork");
	if (pid == 0){
		printf("Child created!\n");
		fflush(stdout);
	}
	if (pid > 0){
		printf("Parent still executing!\n");
		fflush(stdout);
	}

	// Tests exec
	char buff[100];
	sprintf(buff,"%d", bufferSize);
	if (pid == 0){
		int val = execl("./child", "child", "1", "2", buff, NULL);
		if (val == -1) perrorExit("createChild - exec failed");
	}

}

static void waitForChildren(Clock * clock){
	pid_t child;
	while ((child = wait(NULL)) ){
		incrementClock(clock, CLOCK_INCREMENT);
		if ((child == -1) && (errno != EINTR))
			break;
	}
	
}
