// ossOptions.h was created by Mark Renard on 2/21/2020
// This file defines a struct type which is intended to store values entered
// in command line invocations as well as a prototype for a function that
// returns such a struct as a function of the command line arguments.

typedef struct options {
        int numChildrenTotal;
        int simultaneousChildren;
        int beginningIntTested;
        int increment;
        char * outputFileName;
} Options;

Options getOptions(int argc, char * argv[]);
