#include <stdio.h>
#include "cli.h"
#include "utils.h"

// Private Functions //

// Public API //

void parse(int argc, char **argv) {
    // Bounds check
    if (argc > 5 || argc < 2) {
        fprintf(stderr, RED "byteman parse: error: expected 2-5 args got %d\n" TRY_PASSMAN_HELP RESET, argc);
    }
}