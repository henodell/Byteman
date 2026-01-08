#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "commands.h"
#include "crypto.h"

// Private Functions //

// Ask for username login 
void LoginUsername(char *buf, char *file_name, const int BUFFER_SIZE) {
    while (1) {
        printf("Username: ");

        if (ReadInput(buf, BUFFER_SIZE) != 1) {
            fprintf(stderr, RED "byteman input: error: Input error or EOF.\n" TRY_BYTEMAN_HELP RESET);
            exit(1);
        }

        strcpy(file_name, buf);
        strcat(file_name, ".vault");
        
        if (FileExists(file_name)) {
            break;
        }
    }
}

// Write a file into a buffer
char *FileToBuffer(FILE *vault) {
    // get file size
    fseek(vault, 0, SEEK_END);
    long file_size = ftell(vault);
    fseek(vault, 0, SEEK_SET);
    
    if (file_size <= 0) {
        return NULL;
    }

    char *buffer;
    buffer = malloc(file_size + 1);
    if (!buffer) {
        return NULL;
    }

    size_t read = fread(buffer, 1, file_size, vault);
    if (read != (size_t)file_size) {
        free(buffer);
        return NULL;
    }

    // for string use
    buffer[file_size] = 0;
    return buffer;
}

char *GetSalt(FILE *vault) {   
    fseek(vault, 0, SEEK_SET); 
    char *buf = FileToBuffer(vault);

    // get starting position of salt
    char *salt = strchr(buf, ':');
    salt++;
    
    // replace ending : with null
    strtok(salt, ":");
    char *out = strdup(salt);
    free(buf);
    return out;
}

char *GetHash(FILE *vault) {
    fseek(vault, 0, SEEK_SET); 
    char *buf = FileToBuffer(vault);

    // salt then hash
    char *hash = strchr(buf, ':');
    hash++;
    hash = strchr(hash, ':');
    hash++;

    strtok(hash, ":");
    char *out = strdup(hash);
    free(buf);
    return out;

    // for (unsigned int i = 0; i < )
}

void HexDecode(const char *hex, unsigned char *out, size_t out_len) {
    for (size_t i = 0; i < out_len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &out[i]);
    }
}

// Ask for password to login
void LoginPassword(unsigned char *buf, char *file_name, const int BUFFER_SIZE) {
    FILE *vault = fopen(file_name, "r");
    if (!vault) {
        fprintf(stderr, RED "byteman file: error: %s\n" TRY_BYTEMAN_HELP RESET, errno);
        exit(1);
    }

    while (1) {
        printf("Enter your master password: ");
        
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            break;
        }

        char *saltHex = GetSalt(vault);
        char *hash = GetHash(vault);

        // write both back into binary
        char salt[SALT_SIZE];
        HexDecode(saltHex, salt, SALT_SIZE);

        unsigned char *digest = NULL;
        unsigned int md_len = 0;
        DigestMessage(buf, strlen((char *)buf), &digest, &md_len, salt, SALT_SIZE);

        char digest_hex[EVP_MAX_MD_SIZE * 2 + 1];

        for (unsigned int i  = 0; i < md_len; i++) {
            sprintf(digest_hex, "%02x", digest[i]);
        }

        digest_hex[md_len] = 0;

        if (strcmp(digest_hex, hash) == 0) {
            break;
        }
    }
}

// Public API //

/*
@brief Main login handler
*/
void Login() {
    char user_name[20 + 1];
    unsigned char password[64 + 1];
    char file_name[sizeof(user_name) + 6 + 1];
    LoginUsername(user_name, file_name, sizeof(user_name));
    LoginPassword(password, file_name, sizeof(password));
    FILE *vault = fopen(file_name, "r");

    // LoginPassword(password, file_name, sizeof(password));
    printf("Logged in!");
}