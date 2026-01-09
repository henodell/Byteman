#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <openssl/rand.h>
#include "LockedCommands.h"
#include "Utils.h"
#include "Crypto.h"

enum PassError {
    OK = 1,
    TOO_SHORT,
    HAS_SPACE,
    WEAK
};

// Private Functions //

// Error handling
void WriteHandleErrors(void) {
    fprintf(stderr, RED "byteman file: error: fwrite failed: %s\n" TRY_BYTEMAN_HELP RESET, strerror(errno));
    exit(1);
}

// Helpers
void WriteFile(FILE *vault, char *buf, const int SIZE) {
    if (fwrite(buf, 1, SIZE, vault) != SIZE) {
        WriteHandleErrors();
    }
}

// Checks //

// Username checks
int UsernameCheck(char *user) {
    for (int i = 0; user[i]; i++) {
        if (!isalnum((unsigned char) user[i])) {
            return 0;
        }
    }

    return 1;
}

// Password checks
enum PassError PasswordCheck(char *pass) {
    if (strlen(pass) < 8) {
        return TOO_SHORT;
    }

    int digit_check = 0;
    int letter_check = 0;
    int symbol_check = 0;
    // assume no spaces
    int space_check = 0;
    
    for (int i = 0; pass[i]; i++) {
        if (isdigit((unsigned char) pass[i])) {
            digit_check = 1;
        } else if (isalpha((unsigned char) pass[i])) {
            letter_check = 1;
        } else if (ispunct((unsigned char) pass[i])) {
            symbol_check = 1;
        } else if(isspace((unsigned char) pass[i])) {
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

// Input //

// Input for username
void GetUsername(char *buf, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            exit(1);
        }

        // empty strings
        if (buf[0] == 0) {
            continue;
        } 
        
        if (strlen(buf) > USERNAME_MAX) {
            printf(RED "Max 20 chars." RESET);
            continue;
        }

        char f_name[BUFFER_SIZE + 6 + 1];
        strcpy(f_name, buf);
        strcat(f_name, ".vault");
        
        if (FileExists(f_name) == 1) {
            printf(RED "Use a different username.\n" RESET);
            continue;
        }

        if (UsernameCheck(buf) == 1) {
            break;
        } 
    }
}

// Input for password
void GetPassword(char *buf, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a password(max 64 chars, min 8 chars): ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            exit(1);
        }

        enum PassError err = PasswordCheck(buf);

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

// Input for confirm password
void GetPasswordConfirm(char *buf, const int BUFFER_SIZE, char *original) {
    while (1) {
        printf("Confirm your master password: ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            exit(1);
        }

        if (strcmp(buf, original) == 0) {
            break;
        }
    }
}

// Public API //

void Signup(void) {
    char user_name[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];
    char pass_confirm[PASSWORD_MAX + 1];

    GetUsername(user_name, sizeof(user_name));
    GetPassword(password, sizeof(password));
    GetPasswordConfirm(pass_confirm, sizeof(pass_confirm), password);

    // create a file with .vault extension to store pass
    char file_name[sizeof(user_name) + 7];
    snprintf(file_name, sizeof(file_name), "%s.vault", user_name);

    FILE *vault = fopen(file_name, "wb");

    if (!vault) {
        fprintf(stderr, RED "byteman file: error: %s\n", TRY_BYTEMAN_HELP  RESET, errno);
        exit(1);
    }

    printf(GREEN "Successfully signed up!\n" RESET);

    // salt and hashing
    unsigned char *md_value = NULL;
    unsigned int md_len = 0;
    unsigned char salt[SALT_SIZE];
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        fprintf(stderr, RED "byteman pass: error: RAND_bytes failed. " TRY_BYTEMAN_HELP RESET);
        exit(1);
    }

    DigestMessage((unsigned char *) password, strlen(password), &md_value, &md_len, salt, sizeof(salt));

    // remove password memory from stack
    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(pass_confirm, sizeof(pass_confirm));

    /*
    <user_length><user_name><salt><hash>
    */

    uint8_t user_len = strlen(user_name);
    if (fwrite(&user_len, 1, 1, vault) != 1) {
        WriteHandleErrors();
    }

    WriteFile(vault, user_name, user_len);
    WriteFile(vault, salt, SALT_SIZE);
    WriteFile(vault, md_value, md_len);
    
    fclose(vault);
}