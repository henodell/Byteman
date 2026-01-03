#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cli.h"
#include "utils.h"

// Private Functions //

void ToLowerCase(char *s) {
    for (int i = 0; i < strlen(s); i++) {
        s[i] = tolower(s[i]);
    }
}

// Public API //

void ParseArgs(int argc, char **argv) {
    // Bounds check
    if (argc > 5 || argc < 2) {
        fprintf(stderr, RED "byteman parse: error: expected 2-5 args got %d\n" TRY_BYTEMAN_HELP RESET, argc);
    }
    
    char *command = argv[1];
    ToLowerCase(command);
    DoLookup(command);
}