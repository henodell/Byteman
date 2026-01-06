#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include "utils.h"

// Public API //

enum AppState GetAppState() {
    return LOCKED;
    // read contents of .session
}

// Input //

int ReadInput(char *buf, const int BUFFER_SIZE) {
    if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
        return 0;
    }

    buf[strcspn(buf, "\n")] = 0;

    return 1;
}

// Hashing/salting

void DigestMessage(const unsigned char *msg, size_t len, unsigned char **digest, unsigned int *digest_len, char *salt, size_t salt_len) {
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
        printf("Error!\n");
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

    if ((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL) {
        printf("Error!\n");
    }

    if (EVP_DigestFinal_ex(mdctx, *digest, digest_len) != 1) {
        printf("Error!|n");
    }

    EVP_MD_CTX_free(mdctx);
}

int FileExists(const char *name) {
    FILE *f = fopen(name, "r");

    if (f != NULL) {
        return 1;
    }

    return 0;
}

// 


