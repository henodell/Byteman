#pragma once

// Text colours
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

// Errors
#define TRY_BYTEMAN_HELP "try byteman help for more information\n"

// Types
enum AppState {
    LOCKED,
    OPEN
};

enum AppState GetAppState();
int FileExists(const char *name);