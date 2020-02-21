// ossOptions.c was created by Mark Renard on 2/21/2020
//
// This file defines a function which returns an Options structure that
// stores values of command line arguments entered in an invocation of the
// oss utility.

#include "ossOptions.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

static void printHelpMsg(char * exeName);
static void printErrorMsg(char * exeName, char opt);

// Usage message that prints when the user enteres -h, printed by printHelpMesg
const static char * USAGE_FORMAT =
   "Usage: %s [-h] [-n x -s x -b B -i I -o filename]\n"
   " -h           Prints this help message\n"
   " -n x         Maximum total number of child processes (default 4)\n"
   " -s x         Maximum number of simultaneous children (default 2)\n"
   " -b B         Start of the sequence to be tested for primality\n"
   " -i I         Increment between numbers to be tested\n"
   " -o filename  Specifies the name of the output file\n"
   "Type cat README for more information.\n";

// Returns an Options struct which contains the optarg
Options getOptions(int argc, char * argv[]){
	int option; // The current option from getopt

	opterr = 0; // Turns off default error messages

	// Initializes return value to default values
	Options opts = { 4, 2, 1, 1, "output.txt" }; 

	// Cycles through entered options
	while((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
		case 'h':
			printHelpMsg(argv[0]);
			exit(0);
		case 'n':
			printf("option n with value %d\n", atoi(optarg));
			break;
		case 's':
			printf("option s with value %d\n", atoi(optarg));
			break;
		case 'b':
			printf("option b with value %d\n", atoi(optarg));
			break;
		case 'i':
			printf("option i with value %d\n", atoi(optarg));
			break;
		case 'o':
			printf("option o with value %s\n", optarg);
			break;
		case '?':
			printErrorMsg(argv[0], optopt);
			exit(1);
		}
	}

	return opts;
}

// Prints USAGE_FORMAT with exeName to stdout
static void printHelpMsg(char * exeName){
	printf(USAGE_FORMAT, exeName);
}

// Prints an invalid option error message using perror
static void printErrorMsg(char * exeName, char opt){
	char errmsg[100]; // Buffer for perror
	sprintf(errmsg, "%s: Error: %c is an invalid option", exeName, opt);
	errno = 1;
	perror(errmsg);
}

