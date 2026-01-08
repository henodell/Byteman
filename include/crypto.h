#pragma once

#define SALT_SIZE 16

void DigestMessage(const unsigned char *msg, size_t len, unsigned char **digest, unsigned int *digest_len, char *salt, size_t salt_len);
