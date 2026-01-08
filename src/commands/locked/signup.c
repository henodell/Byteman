#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "commands.h"
#include "utils.h"
#include "crypto.h"

enum PassError {
    OK = 1,
    TOO_SHORT,
    HAS_SPACE,
    WEAK
};

// Private Functions //

// Checks //

// Username checks
int UsernameCheck(char *user) {
    for (int i = 0; user[i]; i++) {
        if (!isalnum(user[i])) {
            return 0;
            break;
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
        if (isdigit(pass[i])) {
            digit_check = 1;
        } else if (isalpha(pass[i])) {
            letter_check = 1;
        } else if (ispunct(pass[i])) {
            symbol_check = 1;
        } else if(isspace(pass[i])) {
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

// Password confirm checks
int ConfirmCheck(char *buf, char *original) {
    if (strcmp(buf, original) == 0) {
        return 1;
    }

    return 0;
}

// Input //

// Input for username
void GetUsername(char *buf, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            exit(1);
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

        if (ConfirmCheck(buf, original) == 1) {
            break;
        }
    }
}

// Public API //

/*
@brief Main signup handler
*/
void Signup() {
    char user_name[20 + 1];
    char password[64 + 1];
    char pass_confirm[64 + 1];

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
    RAND_bytes(salt, sizeof(salt));
    DigestMessage((unsigned char *) password, strlen(password), &md_value, &md_len, salt, sizeof(salt));

    /*
    <user_length><user_name><salt><hash>
    */

    uint8_t user_len = strlen(user_name);
    fwrite(&user_len, 1, 1, vault);

    fwrite(user_name, 1, user_len, vault);
    fwrite(salt, 1, SALT_SIZE, vault);
    fwrite(md_value, 1, md_len, vault);
    fclose(vault);
}