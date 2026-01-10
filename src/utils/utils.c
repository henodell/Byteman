#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include "Utils.h"

// Public API //

enum AppState GetAppState(void) {
    return LOCKED;
    // read contents of .session
}

int ReadInput(char *buf, const int BUFFER_SIZE) {
    if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
        return 0;
    }

    buf[strcspn(buf, "\n")] = 0;

    return 1;
}

int FileExists(const char *name) {
    FILE *f = fopen(name, "r");

    if (f != NULL) {
        return 1;
    }

    return 0;
}

void PrintInfoMessage(char *msg) {
    printf("[" BLUE "INFO" RESET "] " "%s\n", msg);
}