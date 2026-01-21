#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/crypto.h>
#include <cJSON.h>

#include "Utils.h"
#include "LockedCommands.h"
#include "Crypto.h"

// Shell //

// Get hash and salt from a vault file
void GetHashAndSalt(cJSON **hash_b64, cJSON **salt_b64, cJSON **root, FILE *vault) {
    // Read file into a string
    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), vault);
    if (len == 0) {
        fprintf(stderr, "Unable to fread file, ", strerror(errno));
        exit(1);
    }
    rewind(vault);

    buffer[len] = 0;

    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Unable to parse JSON data, ", error_ptr);
        }
        cJSON_Delete(json);
        exit(1);
    }

    *root = json;
    *hash_b64 = cJSON_GetObjectItemCaseSensitive(json, "hash");
    *salt_b64 = cJSON_GetObjectItemCaseSensitive(json, "salt");
    
    if (!*hash_b64|| !*salt_b64) {
        fprintf(stderr, "Vault missing salt or hash.\n");
        exit(1);
    }
}

// Login username by checking if a file exists with input
void LoginUsername(char *user, const size_t BUFFER_SIZE, char *file_name, const size_t FILE_BUF_SIZE) {
    while (1) {
        printf("Username: ");
        if (fgets(user, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to possible input error or EOF.\n");
            exit(1);
        }

        FlushStdin(user);
        user[strcspn(user, "\n")] = 0;

        snprintf(file_name, FILE_BUF_SIZE, "%s%s", user, VAULT_EXT);

        if (FileExists(file_name)) {
            break;
        }
    };
}

// Login password by comparing hashes
void LoginPassword(char *pw, const size_t BUFFER_SIZE, struct GlobalFlags *g_flags, FILE *vault) {
    cJSON *hash = NULL;
    cJSON *salt = NULL;
    cJSON *root = NULL;

    GetHashAndSalt(&hash, &salt, &root, vault);
    PrintVerboseMessage("Getting hash and salt from vault file", g_flags);

    unsigned char d_hash[HASH_SIZE];
    int hash_outl = 0;
    DecodeBase64(d_hash, &hash_outl, hash->valuestring, strlen(hash->valuestring));
    PrintVerboseMessage("Successfully decoded hash", g_flags);

    unsigned char d_salt[SALT_SIZE];
    int salt_outl = 0;
    DecodeBase64(d_salt, &salt_outl, salt->valuestring, strlen(salt->valuestring));
    PrintVerboseMessage("Successfully decoded salt", g_flags);

    while (1) {
        printf("Password: ");
        if (fgets(pw, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }

        FlushStdin(pw);
        pw[strcspn(pw, "\n")] = 0;

        unsigned char result[HASH_SIZE];
        PrintVerboseMessage("Deriving hash from password with Argon2id", g_flags);
        DeriveArgon2ID(pw, d_salt, result);        

        PrintVerboseMessage("Comparing hashes.", g_flags);
        if (CRYPTO_memcmp(result, d_hash, sizeof(d_hash)) == 0) {
            break;
        }
    }
}

// Handler
void Login(CommandArgs *args, struct GlobalFlags *g_flags) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char file_name[USERNAME_MAX + sizeof(VAULT_EXT) + 1];

    LoginUsername(user_name, sizeof(user_name), file_name, sizeof(file_name));
    FILE *vault = fopen(file_name, "r");
    if (!vault) {
        fprintf(stderr, "Unable to open a vault in mode \'rb\', %s", strerror(errno));
        exit(1);
    }
    PrintVerboseMessage("Vault opened with mode \'r\'", g_flags);

    LoginPassword(password, sizeof(password), g_flags, vault);
    fclose(vault);

    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(file_name, sizeof(file_name));
    PrintVerboseMessage("Cleansing buffers", g_flags);

    // TODO: Add persistent state and START WORK ON OPEN COMMANDS
    printf(GREEN "Successfully logged in!" RESET);
}