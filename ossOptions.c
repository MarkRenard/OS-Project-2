// ossOptions.c was created by Mark Renard on 2/21/2020
//
// This file defines a function which returns an Options structure that
// stores values of command line arguments entered in an invocation of the
// oss utility.

#include "ossOptions.h"
#include "perrorExit.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


/* Function Prototypes */

static void printHelpMsg(char * exeName);
static void printErrorMsg(char * exeName, char opt);
static int getInt(char * str);


/* Named Constants */

// Usage message that prints when the user enteres -h, printed by printHelpMesg
const static char * USAGE_FORMAT =
 "Usage: %s [-h] [-n x -s x -b B -i I -o filename]\n"
 " -h           Prints this help message\n"
 " -n x         Maximum total number of child processes (default %d)\n"
 " -s x         Maximum number of simultaneous children (default %d, max %d)\n"
 " -b B         Start of the sequence to be tested for primality (default %d)\n"
 " -i I         Increment between numbers to be tested (default %d)\n"
 " -o filename  Specifies the name of the output file (default %s)\n"
 "Type cat README for more information.\n";

// Default values
const char * DEFAULT_N_STR = "4";
const int DEFAULT_N = 4;

const char * DEFAULT_S_STR = "2";
const int DEFAULT_S = 2;

const char * DEFAULT_B_STR = "5";
const int DEFAULT_B = 5;

const char * DEFAULT_I_STR = "2";
const int DEFAULT_I = 2;

// The default name of the log file
const char * DEFAULT_FILE_NAME = "output.log";

// The maximum number of simultaneous processes
const int MAX_SIMULTANEOUS = 20;
const char * MAX_SIMULTANEOUS_STR = "20";


/* Function Definitions */

// Returns an Options struct which contains the optarg
Options getOptions(int argc, char * argv[]){
	int option; // The current option from getopt

	opterr = 0; // Turns off default error messages

	// Initializes return value to default values
	Options opts = {
		DEFAULT_N,
		DEFAULT_N_STR,
		DEFAULT_S,
		DEFAULT_S_STR,
		DEFAULT_B,
		DEFAULT_B_STR,
		DEFAULT_I,
		DEFAULT_I_STR,
		DEFAULT_FILE_NAME
	 }; 

	// Cycles through entered options
	while((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
		case 'h':
			// Prints help and exits on -h
			printHelpMsg(argv[0]);
			exit(0);
		case 'n':
			opts.numChildrenTotal = getInt(optarg);
			opts.numChildrenTotalStr = optarg;
			break;
		case 's':
			opts.simultaneousChildren = getInt(optarg);

			// Validates -s option, replaces with max if invalid
			if (opts.simultaneousChildren > MAX_SIMULTANEOUS){
				printf("Too many simultaneous processes!\n");
				printf("Using max value of %d instead.\n\n",
					MAX_SIMULTANEOUS);
				opts.simultaneousChildren = MAX_SIMULTANEOUS;
				opts.simultaneousChildrenStr = \
					MAX_SIMULTANEOUS_STR;
			} else {
				opts.simultaneousChildrenStr = optarg;
			}	
			break;
		case 'b':
			opts.beginningIntTested = getInt(optarg);
			opts.beginningIntTestedStr = optarg;
			break;
		case 'i':
			opts.increment = getInt(optarg);
			opts.incrementStr = optarg;
			break;
		case 'o':
			opts.outputFileName = optarg;
			break;
		case '?':
			printErrorMsg(argv[0], optopt);
			exit(1);
		}
	}

	return opts;
}

// Returns an int from a string or exits with an error message
static int getInt(char * str){
	int num = atoi(str);

	if (num < 1){
		char buff[100];
		sprintf(buff, "%s is not a valid numerical argument.", str);
		perrorExit(buff);
	}

	return num;
}

// Prints USAGE_FORMAT with exeName to stdout
static void printHelpMsg(char * exeName){
	printf(USAGE_FORMAT, exeName, DEFAULT_N, DEFAULT_S, MAX_SIMULTANEOUS,
		DEFAULT_B, DEFAULT_I, DEFAULT_FILE_NAME);
}

// Prints an invalid option error message using perror
static void printErrorMsg(char * exeName, char opt){
	char errmsg[100]; // Buffer for perror
	sprintf(errmsg, "%s: Error: %c is an invalid option", exeName, opt);
	errno = 1;
	perror(errmsg);
}

