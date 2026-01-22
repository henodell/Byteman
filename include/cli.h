#pragma once

/**
 * @struct GlobalFlags
 * @brief Stores global flags
 */
struct GlobalFlags {
    int version; /**< Gets version of current byteman */
    int verbose; /**< Prints [INFO] messages */
    int help; /**<Writes how to use byteman  */
};

/**
 * @struct CommandArgs
 * @brief Holds all the arguments to pass into commands
 */
typedef struct {
    int argc; /**< Argument count */
    char **argv; /**< Array of arguments */
} CommandArgs;

/**
 * @struct CliParams
 * @brief Holds all params to pass around
 */
struct CliParams {
    char *comm; /**Command to be used */
    CommandArgs *args; /**Argc and argv for command use */
    struct GlobalFlags *g_flags; /**Current global flags */
};

/**
 * @brief Parses command-line arguments
 * 
 * This function bounds checks and then standardises input.
 * Prints an error message for byteman parse on failure.
 * 
 * @param argc Argument count
 * @param argv Array of arguments containing only the arguments for the command
 */
void ParseArgs(int argc, char **argv);

/**
 * @brief Does a lookup to execute the function
 * 
 * This function gets the current AppState and then does a lookup for commands
 * on that AppState to execute.
 * Prints an error message for byteman lookup on failure.
 * 
 * @param params struct containing command, args and global flags
 */
void DoLookup(struct CliParams params);
