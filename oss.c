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
static void cleanUpAndKillEverything(char * shm);

const static Clock CLOCK_INCREMENT = {0, 10000}; // Virtual time increment
const static Clock MAX_TIME = {2, 0};		 // Time limit for children

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
	Clock * clockPtr = initializeClock((Clock *)shm);
	
	printf("BEFORE CREATE CHILDREN\n");
	fflush(stdout);

	// Forks and execs child
	createChildren(opts, bufferSize);
	
	printf("AFTER CREATECHILDREN\n");
	fflush(stdout);
	// Waits for children to finish executing
	waitForChildren(clockPtr);
	
	// Prints value of clock;
	printf("oss - Seconds: %d Nanoseconds: %d\n",
		clockPtr->seconds,
		clockPtr->nanoseconds
	);
	fflush(stdout);

	// Detatches from and removes shared memory segment
	cleanUpAndKillEverything(shm);	

	// Should never execute:
	printf("This line shouldn't execute.");
	sleep(8);

	return 1;
}

// Creates children up to the specified limits
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
		execl("./child", "child", "1", "2", buff, NULL);
		perrorExit("createChild - exec failed");
	}

}

static void waitForChildren(Clock * clock){
	pid_t child;

	printf("Wainting for children - Seconds: %d Nanoseconds: %d\n",
		clock->seconds,
		clock->nanoseconds
	);
	fflush(stdout);

	while ((child = waitpid(-1, NULL, WNOHANG)) == 0 && clockCompare(clock, &MAX_TIME) < 0){

		// Increments and prints clock
		incrementClock(clock, CLOCK_INCREMENT);
		printf("oss - Seconds: %d Nanoseconds: %d\n",
			clock->seconds,
			clock->nanoseconds
		);
		fflush(stdout);
		
		// Breaks if there are no children or non-interrupt error
		if ((child == -1) && (errno != EINTR)) break;
	}
	printf("child: %d, clockCompare: %d\n", (int)child, clockCompare(clock, &MAX_TIME));
	fflush(stdout);
}

static void cleanUpAndKillEverything(char * shm){
	// Detatches from and removes shared memory. Defined in sharedMemory.c
	detach(shm);
	removeSegment(); // shmid is in static variable in sharedMemory.c
	
	// Kills all processes in the same process group
	kill(0, SIGQUIT);
}
