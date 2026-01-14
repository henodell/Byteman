#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include "Utils.h"
#include "Cli.h"

// Public API //

enum AppState GetAppState(void) {
    return LOCKED;
}

int FileExists(const char *name) {
    FILE *f = fopen(name, "r");

    if (f != NULL) {
        return 1;
    }

    return 0;
}

void PrintInfoMessage(char *msg) {
    printf("[INFO] %s\n", msg);
}