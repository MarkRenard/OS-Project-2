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

	printf("Hello from %s process!\n", exeName);
	printf("index: %d\n", index);
	printf("numberToTest: %d\n", numberToTest);
	printf("bufferSize: %d\n", bufferSize);


	Clock * shm = (Clock *)sharedMemory(bufferSize, 0);
	
	
	printf("child - Seconds: %d Nanoseconds: %d\n",
		 shm->seconds,
		 shm->nanoseconds
	);

//	while(1);

	return (char) index;
}
