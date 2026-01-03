#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

// Private Functions //

int file_exists(const char *file_name) {
    FILE *file = fopen(file_name, "r");

    if (file != NULL) {
        return 1;
    }

    return 0;
}

// Public API //

enum app_state get_app_state() {
    if (!file_exists(".session")) {
        // fprintf(stderr ,RED "byteman file: error: %s\n" RESET, strerror(errno));
        return LOCKED;
    }

    // read contents of .session
}
