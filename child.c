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

static const Clock TIME_LIMIT = {0, 1000000};

static Clock startTime;	 // The simulated time when the process began
static Clock maxEndTime; // The time after which the process must terminate

int main(int argc, char * argv[]){
	exeName = argv[0]; 		  // Sets global defined in perrorExit
	int index = atoi(argv[1]);	  // The index of this process
	int numToTest = atoi(argv[2]);	  // The number to test for primality
	int bufferSize = atoi(argv[3]);	  // The size of shared memory buffer

	// Attaches to shared memory and gets clock pointer
	char * shm = sharedMemory(bufferSize, 0);
	const Clock * clock = (Clock *) shm;

	// Pointer to this child's portion of shared results array
	int * result = (int *)(shm + sizeof(Clock) + index * sizeof(int));
	
	// Prints info
	printf( "Hello from %s process!\n"
		"index: %d\n"
		"numToTest: %d\n"
		"bufferSize: %d\n"
		"Seconds: %d Nanoseconds: %d\n\n",
		exeName,
		index,
		numToTest,
		bufferSize,
		clock->seconds,
		clock->nanoseconds
	);
	fflush(stdout);
	
	// while(1);

	// Checks 
	if (isPrime(numToTest)){
		*result = numToTest;
	} else {
		*result = -1 * numToTest;
	}

	return 0;
}

//static int checkPrimeWithTimeLimit(int numToTest, const Clock * clock){
static int isPrime(int n){
	int upperLimit; // Num greater than the least factor not equal to 1

	// Imediately returns false if the number is even or less than 2
	if (n % 2 == 0 || n < 2) return 0;

	// Finds the least integer greater than the square root of n
	for (upperLimit = 3; upperLimit * upperLimit <= n; upperLimit++);

	// Returns false if a factor between 2 and upperLimit is found
	int i;
	for (i = 3; i < upperLimit; i += 2){
		if (n % i == 0) return 0;
	}

	// If no factors have been found, the number is prime
	return 1;

}
