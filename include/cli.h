#pragma once

/**
 * @brief Parses command-line arguments
 * 
 * This function does bounds checks and then standardises input.
 * Prints an error message for byteman parse on failure.
 * 
 * @param argc Argument count
 * @param argv Array of arguments
 */
void ParseArgs(int argc, char **argv);

/**
 * @brief Does a lookup to execute the function
 * 
 * This function gets the current AppState and then does a lookup for commands
 * on that AppState to execute.
 * Prints an error message for byteman lookup on failure.
 * 
 * @param comm Command to execute
 */
void DoLookup(char *comm);
