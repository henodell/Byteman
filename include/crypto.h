#pragma once

/**@brief Size of the salt to be used */
#define SALT_SIZE 16

/**
 * @brief Computes a SHA-256 digest of a message with a salt
 * 
 * @param msg Pointer to a message (must not be NULL)
 * @param len Length of the message
 * @param digest Pointer to a pointer to where the resulting digest will be stored
 * @param digest_len Pointer to an int where the length of the digest will be stored
 * @param salt Pointer to the salt to be used
 * @param salt_len The length of the salt being used (SALT_SIZE)
 */
void DigestMessage(const unsigned char *msg, size_t len, unsigned char **digest, unsigned int *digest_len, char *salt, size_t salt_len);