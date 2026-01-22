#pragma once
#include <stdint.h>
#include "LockedCommands.h"

/**
 * @brief Size of the salt to be used 
 */
#define SALT_SIZE 16

/**
 * @brief Size of Argon2ID key to generate
 */
#define KEY_SIZE 16

/**
 * @brief Size of the vault-key to be generated
 */
#define V_KEY_SIZE 32

/**
 * @brief Size of IV for encryption
 */
#define IV_SIZE 16

/**
 * @brief Function macro to calculate output size of Base64
 */
#define ENCODE_SIZE(n) (4 * (((n) + 2) / 3) + 1)

/**
 * @brief Current version of vault data being used
 */
extern int cur_version;

/**
 * @struct VaultData
 * @brief The data to store in a .vault 
 */
struct VaultData {
    int version; /**< Version of the data for future changes*/
    char user_name[USERNAME_MAX]; /**< Null-terminated username */
    char salt[SALT_SIZE]; /**< Salt */
    unsigned char iv[IV_SIZE]; /**IV used for encryption */
    int cipher_len; /**< Length of the encryption ciphertext */
    unsigned char wrapped_vkey[V_KEY_SIZE]; /**< Wrapped vault key */
};

/**
 * @brief Derives a key using the Argon2ID
 * 
 * @param pw Password to derive from
 * @param salt Salt to be used on password
 * @param result Stores derived key
 */
void DeriveArgon2ID(unsigned char *pw, unsigned char salt[SALT_SIZE],
                    unsigned char result[KEY_SIZE]);

/**
 * @brief Encodes value in Base64
 * 
 * @param out Result of encoding
 * @param outl Length of data encoded
 * @param in Stores data to be encoded
 * @param inl Length of data to be encoded
 */
void EncodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl);

/**
 * @brief Decodes Base64 data
 * 
 * @param out Result of decoding
 * @param outl Length of data decoded
 * @param in Stores data to be decoded
 * @param inl Length of data to be decoded
 */
void DecodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl);

/**
 * @brief Encrypts plaintext in AES-256-CBC
 * 
 * @param plaintext Text to be encrypted
 * @param plaintext_len Length to be encrypted
 * @param key Key used for encrypting
 * @param iv Initialisation vector for encryption
 * @param ciphertext Result of encryption
 */
int EncryptAES(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext);