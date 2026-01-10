#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "LockedCommands.h"
#include "Cli.h"
#include "Utils.h"

struct FuncEntry {
    const char name[8];
    void (*func)(CommandArgs *args, struct GlobalFlags *g_flags);
};

struct FuncEntry locked_functions[] = {
    {"login", Login},
    {"signup", Signup},
    // {"get", Get}
};

// Loop through an array of functions to run it
void LoopStructTbl(struct FuncEntry arr[], int size, struct CliParams params) {
    for (int i = 0; i < size; i++) {
        if (strcmp(params.comm, arr[i].name) == 0) {
            arr[i].func(params.args, params.g_flags);
            return;
        }
    }
    fprintf(stderr, RED "byteman lookup: error: couldn't find command in lookup\n" TRY_BYTEMAN_HELP RESET);
    exit(1);
}

// Public API //

const int NUM_LOCKED_FUNCTIONS = sizeof(locked_functions) / sizeof(locked_functions[0]);

void DoLookup(struct CliParams params) {
    enum AppState cur_state = GetAppState();

    if (cur_state == LOCKED) {
        LoopStructTbl(locked_functions, NUM_LOCKED_FUNCTIONS, params);
    }
        // loopTable()
    // }
}