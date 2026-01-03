#pragma once

// Text colours
#define RED "\033[31m"
#define RESET "\033[0m"

// Errors
#define TRY_BYTEMAN_HELP "try byteman help for more information\n"

// Types
typedef enum {
    LOCKED,
    OPEN
} AppState;

AppState getAppState();