#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cJSON.h>
#include "Utils.h"
#include "LockedCommands.h"
#include "Crypto.h"

enum PassError {
    OK = 1,
    TOO_SHORT,
    HAS_SPACE,
    WEAK
};

// Core //

// Version of data being used
int cur_version = 1;

// Creates vault data to write into a file
struct VaultData CreateVaultData(const char *user_name, unsigned char *pw, unsigned char salt[SALT_SIZE]) {
    struct VaultData v = {0};

    v.version = cur_version;
    snprintf(v.user_name, sizeof(v.user_name), "%s", user_name);

    memcpy(v.salt, salt, SALT_SIZE);
    DeriveArgon2ID(pw, salt, v.hash);

    return v;
}

// Transform struct into JSON
char *VaultDataToJson(struct VaultData *v) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "version", v->version);
    cJSON_AddStringToObject(json, "user_name", v->user_name);

    char salt_b64[ENCODE_SIZE];
    int salt_outl = 0;
    EncodeBase64(salt_b64, &salt_outl, v->salt, SALT_SIZE);

    cJSON_AddStringToObject(json, "salt", salt_b64);

    char hash_b64[ENCODE_SIZE];
    int hash_outl = 0;
    EncodeBase64(hash_b64, &hash_outl, v->hash, HASH_SIZE);

    cJSON_AddStringToObject(json, "hash", hash_b64);

    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);

    return json_str;
}

// Checks username to make sure its alphanumeric and ascii
int UsernameCheck(const char *user) {
    for (int i = 0; user[i]; i++) {
        if (!isalnum((unsigned char) user[i]) || !isascii((unsigned char) user[i])) {
            return 0;
        }
    }

    return 1;
}

// Checks password for enough variety
enum PassError PasswordCheck(const char *pw) {
    if (strlen(pw) < 8) {
        return TOO_SHORT;
    }

    int digit_check = 0;
    int letter_check = 0;
    int symbol_check = 0;

    // assume no spaces
    int space_check = 0;
    
    for (int i = 0; pw[i]; i++) {
        if (isdigit((unsigned char) pw[i])) {
            digit_check = 1;
        } else if (isalpha((unsigned char) pw[i])) {
            letter_check = 1;
        } else if (ispunct((unsigned char) pw[i])) {
            symbol_check = 1;
        } else if(isspace((unsigned char) pw[i])) {
            space_check = 1;
        }
    }

    if (space_check) {
        return HAS_SPACE;
    }

    if (digit_check && letter_check && symbol_check) {
        return OK;
    }

    return WEAK;
}

// Shell //

// Writing json vault data to file
void WriteJsonToFile(FILE *vault, char *json_str) {
    if (fputs(json_str, vault) == EOF) {
        fprintf(stderr, "Unable to write json into file");
        exit(1);
    }
}

// Gets input from username
void GetUsername(char *user, char *file_name, const size_t BUFFER_SIZE, const size_t FILE_BUF_SIZE) {
    do {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (fgets(user, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }

        FlushStdin(user);
        user[strcspn(user, "\n")] = 0;

        snprintf(file_name, FILE_BUF_SIZE, "%s%s", user, VAULT_EXT);
        
        if (FileExists(file_name)) {
            printf("Use a different username.\n");
            continue;
        }
    } while (UsernameCheck(user) != 1);
}

// Gets input for password
void GetPassword(char *pw, const size_t BUFFER_SIZE) {
    while (1) {
        printf("Enter a password(max 64 chars, min 8 chars): ");
        if (fgets(pw, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }
        FlushStdin(pw);
        pw[strcspn(pw, "\n")] = 0;

        enum PassError err = PasswordCheck(pw);

        switch (err) {
            case HAS_SPACE:
                printf("Do not use spaces.\n");
                break;
            case TOO_SHORT:
                printf("Use atleast 8 chars.\n");
                break;
            case WEAK:
                printf("Your password must contain a digit, symbol and a letter.\n");
                break;
        }

        if (err == OK) {
            break;
        }
    }
}

// Gets input for password confirmation
void GetPasswordConfirm(char *pw, const int BUFFER_SIZE, const char *original) {
    do {
        printf("Confirm your master password: ");
        if (fgets(pw, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }

        FlushStdin(pw);
        pw[strcspn(pw, "\n")] = 0;
        
    } while (strcmp(pw, original) != 0);
}

// Handler
void Signup(CommandArgs *args, struct GlobalFlags *g_flags) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char pass_confirm[PASSWORD_MAX + 1];
    char file_name[sizeof(user_name) + sizeof(VAULT_EXT) + 1];

    GetUsername(user_name, file_name, sizeof(user_name), sizeof(file_name));
    GetPassword(password, sizeof(password));
    GetPasswordConfirm(pass_confirm, sizeof(pass_confirm), password);

    unsigned char salt[SALT_SIZE];
    if (RAND_bytes(salt, SALT_SIZE) != 1) {
        fprintf(stderr, "Unable to generate salt, %s\n", ERR_get_error());
        exit(1);
    }
    PrintVerboseMessage("Salt generated", g_flags);

    struct VaultData v = CreateVaultData(user_name, password, salt);
    PrintVerboseMessage("Vault data created", g_flags);

    char *json_str = VaultDataToJson(&v);
    PrintVerboseMessage("Vault data transformed into JSON", g_flags);

    FILE *vault = fopen(file_name, "w");
    if (!vault) {
        fprintf(stderr, "Unable to open .vault file, %s\n", strerror(errno));
        exit(1);
    }

    PrintVerboseMessage("Vault file created", g_flags);

    WriteJsonToFile(vault, json_str);
    cJSON_free(json_str);
    fclose(vault);
    PrintVerboseMessage("Vault data written", g_flags);

    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(pass_confirm, sizeof(pass_confirm));
    OPENSSL_cleanse(&v, sizeof(v));
    PrintVerboseMessage("Buffers cleansed", g_flags);
    printf(GREEN "Successfullly signed up!" RESET);
}