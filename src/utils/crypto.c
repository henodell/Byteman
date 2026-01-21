#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/thread.h>
#include <openssl/kdf.h>
#include <string.h>
#include "Crypto.h"

// Public API

void EncodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl) {
    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    if (!ctx) {
        fprintf(stderr, "EVP_ENCODE_CTX_new() failed. Unable to encode data in base64.\n");
        exit(1);
    }

    EVP_EncodeInit(ctx);

    if (EVP_EncodeUpdate(ctx, out + *outl, outl, in, inl) != 1) {
        fprintf(stderr, "EVP_EncodeUpdate() failed. Unable to encode data in base64.\n");
        EVP_ENCODE_CTX_free(ctx);
        exit(1);
    }

    int final_len = 0;
    EVP_EncodeFinal(ctx, out, &final_len);
    *outl += final_len;

    EVP_ENCODE_CTX_free(ctx);
}

void DecodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl) {
    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    if (!ctx) {
        fprintf(stderr, "EVP_ENCODE_CTX_new() failed. Unable to encode data in base64.\n");
        exit(1);
    }

    EVP_DecodeInit(ctx);

    if (EVP_DecodeUpdate(ctx, out, outl, in, inl) < 0) {
        fprintf(stderr, "EVP_DecodeUpdate() failed. Unable to decode base64 data.\n");
        EVP_ENCODE_CTX_free(ctx);
        exit(1);
    }

    int final_len = 0;
    if (EVP_DecodeFinal(ctx, out, &final_len) != 1) {
        fprintf(stderr, "EVP_DecodeFinal() failed. Unable to decode base64 data.\n");
        EVP_ENCODE_CTX_free(ctx);
        exit(1);
    }

    *outl += final_len;
    EVP_ENCODE_CTX_free(ctx);
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