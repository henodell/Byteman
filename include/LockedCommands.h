#pragma once
#include "Cli.h"

/**
 * @brief Size limit for usernames
 */
#define USERNAME_MAX 20

/**
 * @brief Size limits for password
 */
#define PASSWORD_MAX 64

/**
 * @brief Vault Extension
 */
#define VAULT_EXT ".vault"

/**
 * @brief Executes the "signup" command
 * 
 * Asks the user to create a new account. Validates the input and 
 * stores the info securely in a .vault file. Prints errors on failure. 
 * 
 * @param args - Struct containing argc and argv
 * @param g_flags - Struct containing global flags such as --verbose
 */
void Signup(CommandArgs *args, struct GlobalFlags *g_flags);

void Login(CommandArgs *args, struct GlobalFlags *g_flags);