// child.c was created by Mark Renard on 2/21/2020
//
// This file contains a program which determines the primality of an integer
// and records the result in a shared memory region.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sharedMemory.h"
#include "perrorExit.h"
#include "clock.h"

int main(int argc, char * argv[]){
	exeName = argv[0]; 		  // Sets global defined in perrorExit
	int index = atoi(argv[1]);	  // The index of this process
	int numberToTest = atoi(argv[2]); // The number to test for primality
	int bufferSize = atoi(argv[3]);	  // The size of shared memory buffer

	char * shm = sharedMemory(bufferSize, 0);
	Clock * clock = (Clock *) shm;

	printf( "Hello from %s process!\n"
		"index: %d\n"
		"numberToTest: %d\n"
		"bufferSize: %d\n"
		"Seconds: %d Nanoseconds: %d\n\n",
		exeName,
		index,
		numberToTest,
		bufferSize,
		clock->seconds,
		clock->nanoseconds
	);
	fflush(stdout);
	
	//while(1);

	return 0;

}
