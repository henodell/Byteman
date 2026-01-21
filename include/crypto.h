#pragma once
#include <stdint.h>
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
 * @brief Derives a key using the Argon2ID
 * 
 * @param pw Password to derive from
 * @param salt Salt to be used on password
 * @param result Place to store the result (must be atleast 16 bytes)
 */
void DeriveArgon2ID(unsigned char *pw, unsigned char salt[SALT_SIZE], unsigned char result[HASH_SIZE]);

/**
 * @brief Encodes value in base 64
 * 
 * @param out Pointer to a string to store the encoded data (unsigned char)
 * @param outl Pointer to an integer to store the encoded data length
 * @param in Pointer to a string of the data to encode
 * @param inl Integer holding amount of data to encode
 */
void EncodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl);

/**
 * @brief Decodes base64
 * 
 * @param out Pointer to a string to store the decoded data length
 * @param outl Pointer to an integer to store to the decoded data length
 * @param in Pointer to a string of the base64 data to decode
 * @param inl Integer holding the amount of base64 data to decode
 */
void DecodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl);