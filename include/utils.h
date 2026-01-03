#pragma once

// Text colours
#define RED "\033[31m"
#define RESET "\033[0m"

// Errors
#define TRY_BYTEMAN_HELP "try byteman help for more information\n"

// Types
enum app_state {
    LOCKED,
    OPEN
};

enum app_state get_app_state();