#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "cli.h"
#include "utils.h"

typedef struct {
    const char name[8];
    void (*func)(void);
} FuncEntry;

FuncEntry lockedFunctions[] = {
    {"get", ster},
    {"list", ster}
};

// Private Functions //

void loopTable(FuncEntry arr[], int size, char *command) {
    for (int i = 0; i < size; i++) {
        if (strcmp(command, arr[i].name) == 0) {
            arr[i].func();
            return;
        }
    }
    fprintf(stderr, RED "byteman lookup: error: couldn't find command in lookup\n" TRY_BYTEMAN_HELP RESET, command);
}

// Public API //
const int NUM_LOCKED_FUNCTIONS = sizeof(lockedFunctions) / sizeof(lockedFunctions[0]);

void doLookup(char *command) {
    AppState curState = getAppState();

    if (curState == LOCKED) {
        loopTable(lockedFunctions, NUM_LOCKED_FUNCTIONS, command);
    }
        // loopTable();
    // }
}