#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string.h>
#include "Crypto.h"

#define ITERATIONS 10000
#define KEY_LENGTH 32

void handleErrors(char *msg) {
    fprintf(stderr, "\n%s\n", msg);
    exit(1);
}

// Public API 

void DigestMessage(const unsigned char *msg, size_t len, unsigned char digest[SHA256_DIGEST_LENGTH], unsigned int *digest_len, char salt[SALT_SIZE], size_t salt_len) {
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
        handleErrors("Unable to open a context for hashing");
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        handleErrors("Unable to init context for hashing");
    }

    // salt
    if (EVP_DigestUpdate(mdctx, salt, salt_len) != 1) {
        handleErrors("Unable to hash salt");
    }

    if (EVP_DigestUpdate(mdctx, msg, len) != 1) {
        handleErrors("Unable to hash message");
    }

    if (EVP_DigestFinal_ex(mdctx, digest, digest_len) != 1) {
        handleErrors("Unable to get digest");
    }

    EVP_MD_CTX_free(mdctx);
}

void DeriveKeyPbkdf2(const char *password, const unsigned char salt[SALT_SIZE], unsigned char *key, const unsigned int KEY_LEN) {
    int pass_len = (int) strlen(password);
    if (PKCS5_PBKDF2_HMAC(password, pass_len, salt, SALT_SIZE, ITERATIONS, EVP_sha256(), KEY_LEN, key) != 1) {
        handleErrors("PBKDF2 key derivation failed");
    }
} 