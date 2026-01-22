#pragma once
#include "Cli.h"

/**
 * @brief Size limit for username
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
 * REWRITE NEEDED
 * @brief Executes the "signup" command
 * 
 * @param args - Struct containing argc and argv
 * @param g_flags - Struct containing global flags
 */
void Signup(CommandArgs *args, struct GlobalFlags *g_flags);

void Login(CommandArgs *args, struct GlobalFlags *g_flags);