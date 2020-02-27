// clock.c was created by Mark Renard on 2/21/2020
//
// This file contains an implementation of the function incrementClock which
// adds some number of nanoseconds to the time recorded in a virtual clock.

#include "clock.h"
#include <stdio.h>

#define BILLION 1000000000

// Initializes clock to 0 seconds 0 nanoseconds
Clock * initializeClock(Clock * clockPtr){
        clockPtr->seconds = 0;
        clockPtr->nanoseconds = 0;

        return clockPtr;
}

// Copies the time stored in the second clock to the first
void copyTime(Clock * dest, const Clock * src){
	dest->seconds = src->seconds;
	dest->nanoseconds = src->nanoseconds;
}

// Adds a time increment defined in a clock structure to a clock
void incrementClock(Clock * clock, const Clock increment){	
	clock->seconds += increment.seconds;
	clock->nanoseconds += increment.nanoseconds;

	// Carry operations
	clock->seconds += clock->nanoseconds / BILLION;
	clock->nanoseconds = clock->nanoseconds % BILLION;
}

// Returns -1 if the time on clk1 is less, 1 if it's greater, and 0 if equal
int clockCompare(const Clock * clk1, const Clock * clk2){
	if (clk1->seconds != clk2->seconds){
		// Compares seconds if they are not equal
		if (clk1->seconds < clk2->seconds) return -1;
		if (clk1->seconds > clk2->seconds) return 1;
	} else {
		// Compares nanoseconds if seconds are equal
		if (clk1->nanoseconds < clk2->nanoseconds) return -1;
		if (clk1->nanoseconds > clk2->nanoseconds) return 1;
	}

	// If this statement executes, the two times are equal.
	return 0;
}

void printTime(FILE * fp, const Clock * clock){
	fprintf(fp,
		"Seconds: %d Nanoseconds: %d\n",
		clock->seconds,
		clock->nanoseconds
	);
}
