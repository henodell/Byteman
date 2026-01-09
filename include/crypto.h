#pragma once
#include <openssl/sha.h>
#include "LockedCommands.h"

/**
 * @brief Size of the salt to be used 
 */
#define SALT_SIZE 16

/**
 * @struct Vault Data
 * @brief The data to store in a .vault file
 */
struct VaultData {
    uint8_t version; /**< Version of the data */
    uint8_t user_len; /**< Length of the username */
    char user_name[USERNAME_MAX]; /**< Non null-terminated username */
    unsigned int salt_len; /**< Length of the salt*/
    unsigned char salt[SALT_SIZE]; /**< Salt */
    unsigned int hash_len; /**< Length of the hash */
    unsigned char hash[SHA256_DIGEST_LENGTH]; /**< Hash */
};

/**
 * @brief Computes a SHA-256 digest of a message with a salt
 * 
 * @param msg Pointer to a message (must not be NULL)
 * @param len Length of the message
 * @param digest Char array where the resulting digest will be stored (must be atleast 32)
 * @param digest_len Pointer to an int where the length of the digest will be stored
 * @param salt Pointer to the salt to be used
 * @param salt_len The length of the salt being used (SALT_SIZE)
 */
void DigestMessage(const unsigned char *msg, size_t len, unsigned char digest[SHA256_DIGEST_LENGTH], unsigned int *digest_len, char *salt, size_t salt_len);