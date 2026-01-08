#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "utils.h"

// Private Functions //

// Helper to turn str into lowercase
void ToLowerCase(char *s) {
    for (int i = 0; i < strlen(s); i++) {
        s[i] = tolower(s[i]);
    }
}

// Public API //

/*
@brief Parse command line arguments to run the command
@param argc Argument count
@param char** Arguments
*/
void ParseArgs(int argc, char **argv) {
    // Bounds check
    if (argc > 5 || argc < 2) {
        fprintf(stderr, RED "byteman parse: error: expected 2-5 args got %d\n" TRY_BYTEMAN_HELP RESET, argc);
        exit(1);
    }
    
    char *command = argv[1];
    ToLowerCase(command);
    DoLookup(command);
}