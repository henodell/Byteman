#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cli.h"
#include "utils.h"

// Private Functions //

void toLowerCase(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}

// Public API //

void parseArgs(int argc, char **argv) {
    // Bounds check
    if (argc > 5 || argc < 2) {
        fprintf(stderr, RED "byteman parse: error: expected 2-5 args got %d\n" TRY_BYTEMAN_HELP RESET, argc);
    }
    
    char *command = argv[1];
    toLowerCase(command);
    
    doLookup(command);
}