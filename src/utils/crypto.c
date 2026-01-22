#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/thread.h>
#include <openssl/kdf.h>
#include <openssl/err.h>

#include "Crypto.h"

// Helpers

void HandleErrors(void) {
    ERR_print_errors_fp(stderr);
    exit(1);
}

// Public API

void EncodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl) {
    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    if (!ctx) {
        HandleErrors();
    }

    EVP_EncodeInit(ctx);

    if (EVP_EncodeUpdate(ctx, out + *outl, outl, in, inl) != 1) {
        HandleErrors();
    }

    int final_len = 0;
    EVP_EncodeFinal(ctx, out, &final_len);
    *outl += final_len;

    EVP_ENCODE_CTX_free(ctx);
}

void DecodeBase64(unsigned char *out, int *outl, const unsigned char *in, int inl) {
    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    if (!ctx) {
        HandleErrors();
    }

    EVP_DecodeInit(ctx);

    if (EVP_DecodeUpdate(ctx, out, outl, in, inl) < 0) {
        HandleErrors();
    }

    int final_len = 0;
    if (EVP_DecodeFinal(ctx, out, &final_len) != 1) {
        HandleErrors();
    }

    *outl += final_len;
    EVP_ENCODE_CTX_free(ctx);
}

void DeriveArgon2ID(unsigned char *pw, unsigned char salt[SALT_SIZE],
                    unsigned char result[KEY_SIZE]) 
{
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

    if (EVP_KDF_derive(kctx, &result[0], KEY_SIZE, params) != 1) {
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

    fprintf(stderr, "Unable to use Argon2ID to derive a key");
    exit(1);
}

int EncryptAES(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        HandleErrors();
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        HandleErrors();
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        HandleErrors();
    }

    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        HandleErrors();
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int DecryptAes(void);