// clock.h was created by Mark Renard on 2/21/2020
//
// This file defines a type used to simulate a clock in a shared memory region.

#ifndef CLOCK_H
#define CLOCK_H

typedef struct clock {
	int seconds;
	int nanoseconds;
} Clock;

Clock * initializeClock(Clock * clockPtr);
void incrementClock(Clock * clock, Clock increment);
int clockCompare(const Clock * clk1, const Clock * clk2);

#endif
