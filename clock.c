// clock.c was created by Mark Renard on 2/21/2020
//
// This file contains an implementation of the function incrementClock which
// adds some number of nanoseconds to the time recorded in a virtual clock.

#include "clock.h"
#define BILLION 1000000000

void incrementClock(Clock * clock, int increment){	
	clock->seconds += increment / BILLION;
	clock->nanoseconds += increment % BILLION;
}
