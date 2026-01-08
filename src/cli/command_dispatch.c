#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "cli.h"
#include "utils.h"

struct FuncEntry {
    const char name[8];
    void (*func)(void);
};

struct FuncEntry locked_functions[] = {
    {"login", Login},
    {"signup", Signup},
    {"get", Get}
};

// Private Functions //

// Loop through an array of functions to run it
void LoopStructTbl(struct FuncEntry arr[], int size, char *comm) {
    for (int i = 0; i < size; i++) {
        if (strcmp(comm, arr[i].name) == 0) {
            arr[i].func();
            return;
        }
    }
    fprintf(stderr, RED "byteman lookup: error: couldn't find command in lookup\n" TRY_BYTEMAN_HELP RESET);
    exit(1);
}

// Public API //
const int NUM_LOCKED_FUNCTIONS = sizeof(locked_functions) / sizeof(locked_functions[0]);

/*
@brief Lookup for a command to run
@param comm - command to run
*/
void DoLookup(char *comm) {
    enum AppState cur_state = GetAppState();

    if (cur_state == LOCKED) {
        LoopStructTbl(locked_functions, NUM_LOCKED_FUNCTIONS, comm);
    }
        // loopTable()
    // }
}