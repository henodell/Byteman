#pragma once
#include <openssl/sha.h>
#include <time.h>
#include "LockedCommands.h"

/**
 * @brief Size of the salt to be used 
 */
#define SALT_SIZE 16

/**
 * @brief Current version of vault data being used
 */
extern int cur_version;

/**
 * @struct VaultData
 * @brief The data to store in a .vault 
 */
struct VaultData {
    uint8_t version; /**< Version of the data for future changes*/
    uint8_t user_len; /**< Length of the username */
    char user_name[USERNAME_MAX]; /**< Non null-terminated username */
    unsigned int salt_len; /**< Length of the salt*/
    char salt[SALT_SIZE]; /**< Salt */
    unsigned int hash_len; /**< Length of the hash */
    unsigned char hash[SHA256_DIGEST_LENGTH]; /**< Hash */
};

/**
 * @struct Session
 * @brief Data to store for each session
 */
struct Session {
    char *user_name; /**< Null-terminated username*/
    unsigned char *key; /**< Key derived from master password */
    unsigned char *salt;
    time_t time_stamp; /**< Timestamp for expiration */
};

/**
 * @brief Computes a SHA-256 digest of a message with a salt
 * 
 * @param msg Pointer to a message (must not be NULL)
 * @param len Length of the message
 * @param digest Char array where the resulting digest will be stored (must be atleast 32)
 * @param digest_len Pointer to an int where the length of the digest will be stored
 * @param salt Char array with salt to be used (must be atleast 16)
 * @param salt_len The length of the salt being used (SALT_SIZE)
 */
void DigestMessage(const unsigned char *msg, size_t len, unsigned char digest[SHA256_DIGEST_LENGTH], unsigned int *digest_len, char salt[SALT_SIZE], size_t salt_len);

void DeriveKeyPbkdf2(const char *password, const unsigned char salt[SALT_SIZE], unsigned char *key, const unsigned int KEY_LEN);