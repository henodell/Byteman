#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
// Private Functions //

int fileExists(const char *fileName) {
    FILE *file = fopen(fileName, "r");

    if (file != NULL) {
        return 1;
    }

    return 0;
}

// Public API //

AppState getAppState() {
    if (!fileExists(".session")) {
        // fprintf(stderr ,RED "byteman file: error: %s\n" RESET, strerror(errno));
        return LOCKED;
    }

    // read contents of .session
    


}
