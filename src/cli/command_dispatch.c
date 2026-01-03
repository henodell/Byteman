#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "cli.h"
#include "utils.h"

struct FuncEntry {
    const char name[8];
    void (*func)(void);
};

struct FuncEntry locked_functions[] = {
    {"login", Login},
    {"signup", ster},
    {"exit", ster}
};

// Private Functions //

void LoopStructTbl(struct FuncEntry arr[], int size, char *comm) {
    for (int i = 0; i < size; i++) {
        if (strcmp(comm, arr[i].name) == 0) {
            arr[i].func();
            return;
        }
    }
    fprintf(stderr, RED "byteman lookup: error: couldn't find command in lookup\n" 
            TRY_BYTEMAN_HELP RESET);
}

// Public API //
const int NUM_LOCKED_FUNCTIONS = sizeof(locked_functions) / sizeof(locked_functions[0]);

void DoLookup(char *comm) {
    enum AppState cur_state = GetAppState();

    if (cur_state == LOCKED) {
        LoopStructTbl(locked_functions, NUM_LOCKED_FUNCTIONS, comm);
    }
        // loopTable()
    // }
}