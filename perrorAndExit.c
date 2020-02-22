// perrorAndExit was created by Mark Renard on 2/21/2020

#include <stdio.h>
#include <stdlib.h>

// This function prints an error message in a standard format and exits.
void perrorAndExit(char * exeName, char * msg){
	char errmsg[100];
	sprintf(errmsg, "%s: Error: %s", exeName, msg);
	perror(errmsg);

	exit(1);
}
