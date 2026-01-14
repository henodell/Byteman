#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/thread.h>
#include <openssl/kdf.h>
#include <string.h>
#include "Crypto.h"

// Public API

int EncodeBase64(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len) {
    int written = EVP_EncodeBlock(out, in, in_len);
    if (written < 0 || (size_t) written >= out_len) {
        return 0;
    }

    out[written] = 0;
    return written;
}

void DeriveArgon2ID(unsigned char *pw, unsigned char salt[SALT_SIZE], unsigned char result[HASH_SIZE]) {
    EVP_KDF *kdf = NULL;
    EVP_KDF_CTX *kctx = NULL;
    OSSL_PARAM params[6], *p = params;

    uint32_t lanes = 2, threads = 2, memcost = 65536;

    if (OSSL_set_max_threads(NULL, threads) != 1) {
        goto fail;
    }

    p = params;
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threads);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES, &lanes);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST, &memcost);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
                                             salt,
                                            SALT_SIZE);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD,
                                            pw,
                                            strlen((const char *)pw));
    *p++ = OSSL_PARAM_construct_end();

    if ((kdf = EVP_KDF_fetch(NULL, "ARGON2ID", NULL)) == NULL) {
        goto fail;
    }

    if ((kctx = EVP_KDF_CTX_new(kdf)) == NULL) {
        goto fail;
    }

    if (EVP_KDF_derive(kctx, &result[0], HASH_SIZE, params) != 1) {
        goto fail;
    }

    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(kctx);
    OSSL_set_max_threads(NULL, 0);
    return;

    fail:
    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(kctx);
    OSSL_set_max_threads(NULL, 0);

    fprintf(stderr, "Unable to use argon2 to hash password");
    exit(1);
}