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

void PrintVerboseMessage(char *msg, struct GlobalFlags *g_flags) {
    if (g_flags->verbose) {
        printf("[INFO] ""%s\n", msg);
    }
}

void FlushStdin(char *buf) {
    if (!strchr(buf, '\n')) {
        int c;
        while ((c = getchar()) != EOF && c != '\n');
    }
}