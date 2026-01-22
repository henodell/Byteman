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

enum ConfirmResult {
    CONFIRM_OK = 1,
    CONFIRM_BACK
};

// Core //

// Version of data being used
int cur_version = 1;

// Creates vault data to write into a file
static struct VaultData CreateVaultData(const char *user_name, unsigned char *pw) {
    struct VaultData v = {0};

    v.version = cur_version;
    snprintf(v.user_name, sizeof(v.user_name), "%s", user_name);

    // Salt for Argon2ID
    if (RAND_bytes(v.salt, SALT_SIZE) != 1) {
        fprintf(stderr, "Unable to generate salt, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(1);
    };

    // Derive a random vault-key
    unsigned char vkey[V_KEY_SIZE];
    if (RAND_bytes(vkey, V_KEY_SIZE) != 1) {
        fprintf(stderr, "Unable to generate vkey, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(1);
    }
    
    // Derive the key to encrypt the vault-key
    unsigned char KEK[KEY_SIZE];
    DeriveArgon2ID(pw, v.salt, KEK);

    // Encrypt vault key with kek
    unsigned char iv[IV_SIZE];
    if (RAND_bytes(iv, IV_SIZE) != 1) {
        fprintf(stderr, "Unable to generate IV for encryption, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(1);
    }
    memcpy(v.iv, iv, IV_SIZE);
    v.cipher_len = EncryptAES(vkey, V_KEY_SIZE, KEK, iv, v.wrapped_vkey);

    OPENSSL_cleanse(KEK, sizeof(KEK));
    OPENSSL_cleanse(vkey, sizeof(vkey));
    OPENSSL_cleanse(iv, sizeof(iv));

    return v;
}

// Transform struct into JSON
static char *VaultDataToJson(struct VaultData *v) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "version", v->version);
    cJSON_AddStringToObject(json, "user_name", v->user_name);
    cJSON_AddNumberToObject(json, "cipher_len", v->cipher_len);

    char iv_64[ENCODE_SIZE(IV_SIZE)];
    int iv_outl = 0;
    EncodeBase64(iv_64, &iv_outl, v->iv, IV_SIZE);
    cJSON_AddStringToObject(json, "iv", iv_64);

    char salt_64[ENCODE_SIZE(SALT_SIZE)];
    int salt_outl = 0;
    EncodeBase64(salt_64, &salt_outl, v->salt, SALT_SIZE);
    cJSON_AddStringToObject(json, "salt", salt_64);

    char wvkey_64[ENCODE_SIZE(v->cipher_len)];
    int wvkey_outl = 0;
    EncodeBase64(wvkey_64, &wvkey_outl, v->wrapped_vkey, v->cipher_len);
    cJSON_AddStringToObject(json, "wrapped_vault_key", wvkey_64);

    char *json_str = cJSON_Print(json);
    OPENSSL_cleanse(iv_64, sizeof(iv_64));
    OPENSSL_cleanse(salt_64, sizeof(salt_64));
    OPENSSL_cleanse(wvkey_64, sizeof(wvkey_64));

    cJSON_Delete(json);

    return json_str;
}

// Checks username to make sure its alphanumeric and ascii
static int UsernameCheck(const char *user) {
    for (int i = 0; user[i]; i++) {
        if (!isalnum((unsigned char) user[i]) || !isascii((unsigned char) user[i])) {
            return 0;
        }
    }

    return 1;
}

// Checks password for enough variety
static enum PassError PasswordCheck(const char *pw) {
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
static void WriteJsonToFile(FILE *vault, char *json_str) {
    if (fputs(json_str, vault) == EOF) {
        fprintf(stderr, "Unable to write json into file");
        exit(1);
    }
}

// Gets input from username
static void GetUsername(char *user, char *file_name, const size_t BUFFER_SIZE, const size_t FILE_BUF_SIZE) {
    int valid;
    do {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (fgets(user, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }

        FlushStdin(user);
        user[strcspn(user, "\n")] = 0;

        snprintf(file_name, FILE_BUF_SIZE, "%s%s", user, VAULT_EXT);

        valid = UsernameCheck(user);
        if (!valid) {
            printf("Username must be alphanumeric and 20 chars maximum.\n");
            continue;
        }

        if (FileExists(file_name)) {
            valid = 0;
            printf("Vault already exists. Use a different username.\n");
            continue;
        }
    } while (!valid);
}

// Gets input for password
static void GetPassword(char *pw, const size_t BUFFER_SIZE) {
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
static enum ConfirmResult GetPasswordConfirm(char *pw, const int BUFFER_SIZE, const char *original) {
    while (1) {
        printf("To return to the start, type \"back\".\n");
        printf("Confirm your master password: ");
        if (fgets(pw, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "\nUnable to read input due to input error or EOF.\n");
            exit(1);
        }

        FlushStdin(pw);
        pw[strcspn(pw, "\n")] = 0;

        if (strcmp(pw, "back") == 0) {
            return CONFIRM_BACK;
        }

        if (strcmp(pw, original) != 0) {
            printf("Passwords do not match!\n");
            continue;
        }  

        return CONFIRM_OK;
    } 
}

// Handler
void Signup(CommandArgs *args, struct GlobalFlags *g_flags) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char pass_confirm[PASSWORD_MAX + 1];
    char file_name[sizeof(user_name) + sizeof(VAULT_EXT) + 1];

    GetUsername(user_name, file_name, sizeof(user_name), sizeof(file_name));
    while (1) {
        GetPassword(password, sizeof(password));

        enum ConfirmResult res = GetPasswordConfirm(pass_confirm, sizeof(pass_confirm), password);
        if (res == CONFIRM_OK) {
            break;
        }
    }

    struct VaultData v = CreateVaultData(user_name, password);
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
    OPENSSL_cleanse(json_str, strlen(json_str));
    cJSON_free(json_str);
    fclose(vault);

    PrintVerboseMessage("Vault data written", g_flags);

    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(pass_confirm, sizeof(pass_confirm));
    OPENSSL_cleanse(&v, sizeof(v));

    PrintVerboseMessage("Cleansing buffers", g_flags);
    printf(GREEN "Successfully signed up!" RESET);
}