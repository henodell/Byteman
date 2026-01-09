#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "LockedCommands.h"
#include "Utils.h"
#include "Crypto.h"

#define VAULT_EXT ".vault"

enum PassError {
    OK = 1,
    TOO_SHORT,
    HAS_SPACE,
    WEAK
};

// Core //

void PrintInfoMessage(char *msg) {
    printf("[" BLUE "INFO" RESET "] " "%s\n", msg);
}

// Creates vault data to write into a file
struct VaultData CreateVaultData(const char *user_name, const char *pw, unsigned char salt[SALT_SIZE]) {
    struct VaultData v = {0};

    v.user_len = strlen(user_name);
    strcpy(v.user_name, user_name);
    
    v.salt_len = SALT_SIZE;
    memcpy(v.salt, salt, SALT_SIZE);
    DigestMessage((unsigned char *)pw, strlen(pw), v.hash, &v.hash_len, salt, SALT_SIZE);

    return v;
}

// Input and Check Functions //

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

// Writing vault data to file
int WriteVaultData(FILE *vault, struct VaultData *v) {
    if (fwrite(v, sizeof(*v), 1, vault) != 1) {
        return 0;
    }

    return 1;
}


// Gets input from username
void GetUsername(char *user, const size_t BUFFER_SIZE) {
    while (1) {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (!ReadInput(user, BUFFER_SIZE)) {
            exit(1);
        }

        // handle empty strings
        if (user[0] == 0) {
            continue;
        }

        char file_name[USERNAME_MAX + sizeof(VAULT_EXT) + 1];
        snprintf(file_name, sizeof(file_name), "%s.vault", user);
        
        if (FileExists(file_name)) {
            printf(RED "Use a different username.\n" RESET);
            continue;
        }

        if (UsernameCheck(user)) {
            break;
        } 
    }
}

// Gets input for password
void GetPassword(char *pw, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a password(max 64 chars, min 8 chars): ");
        if (!ReadInput(pw, BUFFER_SIZE)) {
            exit(1);
        }

        enum PassError err = PasswordCheck(pw);

        switch (err) {
            case HAS_SPACE:
                printf(RED "Do not use spaces.\n" RESET);
                break;
            case TOO_SHORT:
                printf(RED "Use atleast 8 chars.\n" RESET);
                break;
            case WEAK:
                printf(RED "Your password must contain a digit, symbol and a letter.\n" RESET);
                break;
        }

        if (err == OK) {
            break;
        }
    }
}

// Gets input for password confirmation
void GetPasswordConfirm(char *pw, const int BUFFER_SIZE, char *original) {
    while (1) {
        printf("Confirm your master password: ");
        if (!ReadInput(pw, BUFFER_SIZE)) {
            exit(1);
        }

        if (strcmp(pw, original) == 0) {
            break;
        }
    }
}

// Handler
void Signup(void) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char pass_confirm[PASSWORD_MAX + 1];

    GetUsername(user_name, sizeof(user_name));
    GetPassword(password, sizeof(password));
    GetPasswordConfirm(pass_confirm, sizeof(pass_confirm), password);

    // create a file with .vault extension to store info
    char file_name[sizeof(user_name) + 7];
    snprintf(file_name, sizeof(file_name), "%s.vault", user_name);

    FILE *vault = fopen(file_name, "wb");
    if (!vault) {
        fprintf(stderr, RED "byteman file: error: %s\n", TRY_BYTEMAN_HELP  RESET, strerror(errno));
        exit(1);
    }

    PrintInfoMessage("Vault file created");

    unsigned char salt[SALT_SIZE];
    if (RAND_bytes(salt, SALT_SIZE) != 1) {
        fprintf(stderr, RED "byteman rand: error: %s\n", TRY_BYTEMAN_HELP RESET, ERR_get_error());
        exit(1);
    }

    struct VaultData v = CreateVaultData(user_name, password, salt);
    PrintInfoMessage("Creating vault data");

    if (WriteVaultData(vault, &v) != 1) {
        fprintf(stderr, RED "byteman file: error: %s\n", TRY_BYTEMAN_HELP RESET, strerror(errno));
        exit(1);
    }
    PrintInfoMessage("Writing vault data");

    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(pass_confirm, sizeof(pass_confirm));
    OPENSSL_cleanse(&v, sizeof(v));
    PrintInfoMessage("Cleansing buffers");

    fclose(vault);
    printf(GREEN "Successfully signed up!" RESET);
}