#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

// Private Functions //

int FileExists(const char *name) {
    FILE *f = fopen(name, "r");

    if (f != NULL) {
        return 1;
    }

    return 0;
}

// Public API //

enum AppState GetAppState() {
    if (!FileExists(".session")) {
        // fprintf(stderr ,RED "byteman file: error: %s\n" RESET, strerror(errno));
        return LOCKED;
    }

    // read contents of .session
}
