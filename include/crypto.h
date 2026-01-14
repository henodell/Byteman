#pragma once
#include <openssl/sha.h>
#include <time.h>
#include "LockedCommands.h"

/**
 * @brief Size of the salt to be used 
 */
#define SALT_SIZE 16

/**
 * @brief Size of Argon2ID hash to generate
 */
#define HASH_SIZE 16

/**
 * @brief Size of Base64 encoding
 */
#define ENCODE_SIZE 25

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
    char user_name[USERNAME_MAX]; /**< Null-terminated username */
    char salt[SALT_SIZE]; /**< Salt */
    unsigned char hash[HASH_SIZE]; /**< Hash */
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
 * @brief Derives a key using the Argon2ID
 * 
 * @param pwd Password to derive from
 * @param salt Salt to be used on password
 * @param result Place to store the result (must be atleast 16 bytes)
 */
void DeriveArgon2ID(unsigned char *pw, unsigned char salt[SALT_SIZE], unsigned char result[HASH_SIZE]);

/**
 * @brief 
 */
int EncodeBase64(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len);