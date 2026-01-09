#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <errno.h>
#include "Utils.h"
#include "LockedCommands.h"
#include "Crypto.h"

// Private Functions //

// Error Handling //

void ReadHandleErrors(void) {
    fprintf(stderr, RED "byteman read: error: fread failed\n" TRY_BYTEMAN_HELP RESET);
    exit(1);
}

// Helper //

void ReadFile(FILE *vault, char *buf, const int SIZE) {
    if (fread(buf, 1, SIZE, vault) != SIZE) {
        ReadHandleErrors();
    }
}

void GetDigestAndSalt(FILE *vault, char *salt, char *hash) {
    uint8_t user_len;
    if (fread(&user_len, 1, 1, vault) != 1) {
        ReadHandleErrors();
    }
    fseek(vault, user_len, SEEK_CUR);
    
    ReadFile(vault, salt, SALT_SIZE);
    ReadFile(vault, hash, SHA256_DIGEST_LENGTH);
    fclose(vault);
}

// Input

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
            FILE *vault = fopen(file_name, "rb");
            if (!vault) {
                fprintf(stderr, RED "byteman file: error: %s\n" TRY_BYTEMAN_HELP RESET, strerror(errno));
                exit(1);
            }

            uint8_t user_len;
            if (fread(&user_len, 1, 1, vault) != 1) {
                ReadHandleErrors();
            }

            char user_name[USERNAME_MAX];
            ReadFile(vault, user_name, user_len);
            fclose(vault);

            if (strcmp(user_name, buf) == 0) {
                break;
            }
        }
    }
}

void LoginPassword(char *buf, char *file_name, const int BUFFER_SIZE) {
    while (1) {
        printf("Password: ");

        if (ReadInput(buf, BUFFER_SIZE) != 1) {
            fprintf(stderr, RED "byteman input: error: Input error or EOF.\n" TRY_BYTEMAN_HELP RESET);
            exit(1); 
        }


        if (FileExists(file_name)) {
            FILE *vault = fopen(file_name, "rb");

            if (!vault) {
                fprintf(stderr, RED "byteman file: error: %s\n" TRY_BYTEMAN_HELP RESET, strerror(errno));
                exit(1);
            }

            unsigned char *md_value = NULL;
            unsigned int md_len = 0;

            unsigned char cur_hash[SHA256_DIGEST_LENGTH];
            unsigned char cur_salt[SALT_SIZE];
            GetDigestAndSalt(vault, cur_salt, cur_hash); 
            DigestMessage((unsigned char *)buf, strlen(buf), &md_value, &md_len, cur_salt, SALT_SIZE);

            if (memcmp(md_value, cur_hash, md_len) == 0) {
                break;
            }
        }
    }
}

// Public API //

void Login() {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char file_name[sizeof(user_name) + 6 + 1];
    
    LoginUsername(user_name, file_name, sizeof(user_name));
    LoginPassword(password, file_name, sizeof(password));


    printf(GREEN "Successfully Logged in!" RESET);
}