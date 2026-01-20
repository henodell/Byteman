#pragma once
#include "Crypto.h"

/**
 * @defgroup Terminal Colours ANSI colour escape sequences
 */
/**@{ */

/** @brief ANSI colour escape sequence for green */
#define GREEN "\033[32m"

/** @brief ANSI colour escape sequence to reset terminal formatting */
#define RESET "\033[0m"
/**@} */

/**
 * @enum AppState
 * @brief Represents if the user is authenticated
 */
enum AppState {
    LOCKED, /**< User is not authenticated */
    OPEN /**< User is authenticated */
};

/**
 * @brief Gets the current AppState
 * 
 * Determines whether the user is authenticated or not by
 * validating timestamp and token in `_byteman.session`.
 * 
 * @return OPEN if timestamp and token match
 * @return LOCKED otherwise
 */
enum AppState GetAppState(void);

/**
 * @brief Checks if a file exists
 * 
 * @param name Name of file to check for
 * @return 1 if file exists
 * @return 0 if file doesn't exist
 */
int FileExists(const char *name);

/**
 * @brief Prints out an info message
 * Prints out an information message with the INFO being blue
 * 
 * @param msg Message to print
 * @param g_flags Current global flags data
 */
void PrintVerboseMessage(char *msg, struct GlobalFlags *g_flags);

/**
 * @brief Conditionally Flushes stdin so fgets doesn't read left over input
 * 
 * Checks if there is a newline character in the buffer. If there is a newline character,
 * it doesn't flush since that means all input was read else it flushes.
 * 
 * @param buf The buffer to check for a newline char
 */
void FlushStdin(char *buf);
