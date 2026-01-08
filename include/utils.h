#pragma once

/**
 * @defgroup Terminal Colours ANSI colour escape sequences
 */
/**@{ */
/** @brief ANSI colour escape sequence for red */
#define RED "\033[31m"

/** @brief ANSI colour escape sequence for green */
#define GREEN "\033[32m"

/** @brief ANSI colour escape sequence to reset terminal formatting */
#define RESET "\033[0m"
/**@} */


/**
 * @brief Help hint shown after user errors
 */
#define TRY_BYTEMAN_HELP "try byteman help for more information\n"

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
 * @brief Reads input safely
 * 
 * Uses \c fgets() to read input and replaces trailing newline 
 * with a null terminator.
 * 
 * @param buf Buffer to read into (must be atleast BUFFER_SIZE bytes)
 * @param BUFFER_SIZE Size of buffer
 * @return 1 on success 
 * @return 0 on failure (EOF or input error)
 */
int ReadInput(char *buf, const int BUFFER_SIZE);