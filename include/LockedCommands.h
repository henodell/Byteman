#pragma once

/**
 * @defgroup User Limits
 */
/**@{*/

/**
 * @brief Size limit for usernames
 */
#define USERNAME_MAX 20
/**
 * @brief Size limits for password
 */
#define PASSWORD_MAX 64
/**@} */

/**
 * @brief Executes the "signup" command
 * 
 * Asks the user to create a new account. Validates the input and 
 * stores the info securely in a .vault file. Prints errors on failure. 
 */
void Signup(void);

void Login(void);