#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include "utils.h"

// Public API //

/*
@brief Current state of program based on data from files
@return AppState LOCKED or OPEN
*/
enum AppState GetAppState() {
    return LOCKED;
    // read contents of .session
}

// Input //

/*
@brief Helper to read input
@param buf buffer to read into
@param BUFFER_SIZE size of buffer
@return int 1 on SUCCESS, 0 on FAILURE
*/
int ReadInput(char *buf, const int BUFFER_SIZE) {
    if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
        return 0;
    }

    buf[strcspn(buf, "\n")] = 0;

    return 1;
}

/*
@brief Checks if a file exists
@param name - name of file
@return int 1 on SUCCESS, 0 on FAILURE
*/
int FileExists(const char *name) {
    FILE *f = fopen(name, "r");

    if (f != NULL) {
        return 1;
    }

    return 0;
}

// 


