#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "Crypto.h"

void DigestMessage(const unsigned char *msg, size_t len, unsigned char digest[SHA256_DIGEST_LENGTH], unsigned int *digest_len, char *salt, size_t salt_len) {
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
        // todo
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        printf("Error!\n");
    }

    // salt
    if (EVP_DigestUpdate(mdctx, salt, salt_len) != 1) {
        printf("Error!\n");
    }

    if (EVP_DigestUpdate(mdctx, msg, len) != 1) {
        printf("Error!\n");
    }

    if (EVP_DigestFinal_ex(mdctx, digest, digest_len) != 1) {
        printf("Error!|n");
    }

    EVP_MD_CTX_free(mdctx);
}