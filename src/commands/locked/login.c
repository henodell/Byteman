#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <errno.h>
#include "Utils.h"
#include "LockedCommands.h"
#include "Crypto.h"

// Shell //

// Gets input for username and opens a vault with rb
char *LoginUsername(char *buf, const size_t BUFFER_SIZE) {
    while (1) {
        printf("Username: ");
        if (ReadInput(buf, BUFFER_SIZE) != 1) {
            fprintf(stderr, "\nUnable to read input due to possible input error or EOF.\n");
            exit(1);
        }
        char file_name[USERNAME_MAX + sizeof(VAULT_EXT) + 1];
        snprintf(file_name, sizeof(file_name), "%s%s", buf, VAULT_EXT);

        if (FileExists(file_name)) {
            char *name = strdup(file_name);
            return name;
        }
    }
}

// Gets the vault data and returns the struct filled with it
struct VaultData GetVaultData(FILE *vault) {
    struct VaultData v = {0};
    
    // verify file size
    fseek(vault, 0, SEEK_END);
    long size = ftell(vault);
    rewind(vault);

    if (size != sizeof(struct VaultData)) {
        fprintf(stderr, "Invalid file size, make sure you sign up and login on the same version of byteman.\n");
        exit(1);
    }

    if (fread(&v, sizeof(v), 1, vault) != 1) {
        fprintf(stderr, "Unable to read from vault file, %s", strerror(errno));
        exit(1);
    }

    return v;
}   

// Get input for password and verify against password
void LoginPassword(char *buf, const size_t BUFFER_SIZE, FILE *vault) {
    while (1) {
        printf("Password: ");

        if (ReadInput(buf, BUFFER_SIZE) != 1) {
            fprintf(stderr, "\nUnable to read input due to possible input error or EOF.\n");
            exit(1);
        }

        struct VaultData v = GetVaultData(vault);
        
        if (v.version != cur_version) {
            fprintf(stderr, "Version of data does not match." 
                "Ensure you used the same version of byteman for both signup and login");
            exit(1);
        }


    }
} 

void Login(CommandArgs *args, struct GlobalFlags *g_flags) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    FILE *vault = NULL;

    char *file_name = LoginUsername(user_name, sizeof(user_name));
    vault = fopen(file_name, "rb");
    if (!vault) {
        fprintf(stderr, "Unable to open a file in mode \'rb\', %s", strerror(errno));
        exit(1);
    }

    free(file_name);

    LoginPassword(password, sizeof(password), vault);

    printf(GREEN "Successfully Logged in!" RESET);
}