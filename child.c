// child.c was created by Mark Renard on 2/21/2020
//
// This program which determines the primality of an integer
// and encodes the result in a shared memory region.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sharedMemory.h"
#include "perrorExit.h"
#include "clock.h"

static int isPrime(int numToTest);
static void checkTime();

static const Clock TIME_LIMIT = {0, 1000000};

static Clock endTime;  	// The time after which the process must terminate
static int childNum;	// The logical identifier for the process
static char * shm;	// The shared memory region
static int * result;	// Pointer to shared int for this process

static const Clock * sharedClock;	  // Pointer to the simulated time

int main(int argc, char * argv[]){
	exeName = argv[0]; 		  // Sets global defined in perrorExit
	childNum = atoi(argv[1]);	  // The logical identifier of this process
	int numToTest = atoi(argv[2]);	  // The number to test for primality
	int bufferSize = atoi(argv[3]);	  // The size of shared memory buffer

	// Attaches to shared memory and gets clock pointer
	shm = sharedMemory(bufferSize, 0);
	sharedClock = (Clock *) shm;
	
	// Records current time and sets max end time
	copyTime(&endTime, sharedClock);
	incrementClock(&endTime, TIME_LIMIT);

	// Sets pointer to this child's portion of shared results array
	result = (int *)(shm + sizeof(Clock) + childNum * sizeof(int));
	
	// Prints info
	printf( "Hello from %s process!\n"
		"childNum: %d\n"
		"numToTest: %d\n"
		"bufferSize: %d\n"
		"Seconds: %d Nanoseconds: %d\n\n",
		exeName,
		childNum,
		numToTest,
		bufferSize,
		sharedClock->seconds,
		sharedClock->nanoseconds
	);
	fflush(stdout);

	// Checks primality 
	if (isPrime(numToTest)){
		*result = numToTest;
	} else {
		*result = -1 * numToTest;
	}

	detach(shm);

	return 0;
}

//static int checkPrimeWithTimeLimit(int numToTest, const Clock * clock){
static int isPrime(int n){
	int upperLimit; // Num greater than the least factor not equal to 1

	checkTime();

	// Returns true if n is 2
	if (n == 2) return 1;
	checkTime();

	// Returns false if the number is even or less than 2
	if (n % 2 == 0 || n < 2) return 0;
	checkTime();

	// Finds the least integer greater than the square root of n
	for (upperLimit = 3; upperLimit * upperLimit <= n; upperLimit++){
		checkTime();
	}

	// Returns false if a factor between 2 and upperLimit is found
	int i;
	for (i = 3; i < upperLimit; i += 2){
		if (n % i == 0) return 0;
		checkTime();
	}
	
	/* DEBUG */
	while(1) checkTime();

	// If no factors have been found, the number is prime
	return 1;
}

// Records -1, detaches from main memory, and exits if time limit reached
static void checkTime(){
	if (clockCompare(&endTime, sharedClock) <= 0 ){
		*result = -1;
		detach(shm);
		exit(1);
	}
}	
		
