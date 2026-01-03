#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "cli.h"
#include "utils.h"

struct func_entry {
    const char name[8];
    void (*func)(void);
};

struct func_entry locked_functions[] = {
    {"login", login},
    {"signup", ster},
    {"exit", ster}
};

// Private Functions //

void loop_struct_tbl(struct func_entry arr[], int size, char *comm) {
    for (int i = 0; i < size; i++) {
        if (strcmp(comm, arr[i].name) == 0) {
            arr[i].func();
            return;
        }
    }
    fprintf(stderr, RED "byteman lookup: error: couldn't find command in lookup\n" TRY_BYTEMAN_HELP RESET);
}

// Public API //
const int NUM_LOCKED_FUNCTIONS = sizeof(locked_functions) / sizeof(locked_functions[0]);

void do_lookup(char *comm) {
    enum app_state cur_state = get_app_state();

    if (cur_state == LOCKED) {
        loop_struct_tbl(locked_functions, NUM_LOCKED_FUNCTIONS, comm);
    }
        // loopTable()
    // }
}