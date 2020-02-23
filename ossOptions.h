// ossOptions.h was created by Mark Renard on 2/21/2020
// This file defines a struct type which is intended to store values entered
// in command line invocations as well as a prototype for a function that
// returns such a struct as a function of the command line arguments.

typedef struct options {
        int numChildrenTotal;
        const char * numChildrenTotalStr;

        int simultaneousChildren;
        const char * simultaneousChildrenStr;

        int beginningIntTested;
        const char * beginningIntTestedStr;

        int increment;
        const char * incrementStr;
	
        const char * outputFileName;
} Options;

/* Function Prototype */
Options getOptions(int argc, char * argv[]);

/* Named constants */
extern const char * DEFAULT_N_STR;
extern const int DEFAULT_N;

extern const char * DEFAULT_S_STR;
extern const int DEFAULT_S;

extern const char * DEFAULT_B_STR;
extern const int DEFAULT_B;

extern const char * DEFAULT_I_STR;
extern const int DEFAULT_I;

extern const int MAX_SIMULTANEOUS;
extern const char * MAX_SIMULTANEOUS_STR;

