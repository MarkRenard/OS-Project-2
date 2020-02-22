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

static void printHelpMsg(char * exeName);
static void printErrorMsg(char * exeName, char opt);
static int getInt(char * str);
// Usage message that prints when the user enteres -h, printed by printHelpMesg
const static char * USAGE_FORMAT =
 "Usage: %s [-h] [-n x -s x -b B -i I -o filename]\n"
 " -h           Prints this help message\n"
 " -n x         Maximum total number of child processes (default 4)\n"
 " -s x         Maximum number of simultaneous children (default 2)\n"
 " -b B         Start of the sequence to be tested for primality (default 5)\n"
 " -i I         Increment between numbers to be tested (default 2)\n"
 " -o filename  Specifies the name of the output file (default %s)\n"
 "Type cat README for more information.\n";

// The default name of the log file
const char * DEFAULT_FILE_NAME = "output.log";

// Returns an Options struct which contains the optarg
Options getOptions(int argc, char * argv[]){
	int option; // The current option from getopt

	opterr = 0; // Turns off default error messages

	// Initializes return value to default values
	Options opts = { 4, 2, 5, 2, NULL }; 

	// Cycles through entered options
	while((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
		case 'h':
			printHelpMsg(argv[0]);
			exit(0);
		case 'n':
			printf("option n with value %d\n", atoi(optarg));
			opts.numChildrenTotal = getInt(optarg);
			break;
		case 's':
			printf("option s with value %d\n", atoi(optarg));
			opts.simultaneousChildren = getInt(optarg);
			break;
		case 'b':
			printf("option b with value %d\n", atoi(optarg));
			opts.beginningIntTested = getInt(optarg);
			break;
		case 'i':
			printf("option i with value %d\n", atoi(optarg));
			opts.increment = getInt(optarg);
			break;
		case 'o':
			printf("option o with value %s\n", optarg);
			opts.outputFileName = optarg;
			break;
		case '?':
			printErrorMsg(argv[0], optopt);
			exit(1);
		}
	}

	printf("n - %d\ns - %d\nb - %d\ni = %d\no - %s\n",
		opts.numChildrenTotal,
		opts.simultaneousChildren,
		opts.beginningIntTested,
		opts.increment,
		opts.outputFileName
	);

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
	printf(USAGE_FORMAT, exeName, DEFAULT_FILE_NAME);
}

// Prints an invalid option error message using perror
static void printErrorMsg(char * exeName, char opt){
	char errmsg[100]; // Buffer for perror
	sprintf(errmsg, "%s: Error: %c is an invalid option", exeName, opt);
	errno = 1;
	perror(errmsg);
}

