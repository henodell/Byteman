#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "utils.h"

enum PassError {
    OK = 1,
    TOO_SHORT,
    HAS_SPACE,
    NOT_COMPLEX,
};

// Private Functions //

// Helpers //

int ReadInput(char *buf, const int BUFFER_SIZE) {
    if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
        return 0;
    }

    buf[strcspn(buf, "\n")] = 0;

    return 1;
}

// Checks //

int UsernameCheck(char *user) {
    for (int i = 0; user[i]; i++) {
        if (!isalpha(i)) {
            return 0;
            break;
        }
    }

    return 1;
}

enum PassError PasswordCheck(char *pass) {
    if (strlen(pass) < 8) {
        return TOO_SHORT;
    }

    int digit_check = 0;
    int letter_check = 0;
    int symbol_check = 0;
    // assume no spaces
    int space_check = 1;
    
    for (int i = 0; i < pass[i]; i++) {
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

    if (digit_check && letter_check && space_check) {
        return OK;
    }

    return NOT_COMPLEX;
}

// Input //

void GetUsername(char *buf, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a username(20 chars, alphanumeric): ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            break;
        }

        if (UsernameCheck(buf) == 1) {
            break;
        } 
    }
}

void GetPassword(char *buf, const int BUFFER_SIZE) {
    while (1) {
        printf("Enter a password(max 64 chars, min 8 chars): ");
        if (ReadInput(buf, BUFFER_SIZE) == 0) {
            break;
        }

        enum PassError err = PasswordCheck(buf);

        switch (err) {
            case HAS_SPACE:
                printf(RED "Do not use spaces.\n" RESET);
                break;
            case TOO_SHORT:
                printf(RED "Use atleast 8 chars.\n" RESET);
                break;
            case NOT_COMPLEX:
                printf(RED "Your password must contain a digit, symbol and a letter.\n" RESET);
                break;
        }

        if (err == OK) {
            break;
        }
    }
}

void GetPasswordConfirm(char *buf, const int BUFFER_SIZE) {
    while (1) {}
}




// Public API //

void Signup() {
    char user_name[20 + 1];
    char password[64 + 1];
    char pass_confirm[64 + 1];

    GetUsername(user_name, sizeof(user_name));
    GetPassword(password, sizeof(password));


    printf(GREEN "Successfully signed up!\n" RESET);
    // FILE *vault = fopen(file_name, "a");

    // if (!vault) {
    //     fprintf(stderr, RED "byteman file: error: %s\n", TRY_BYTEMAN_HELP  RESET, errno);
    //     exit(1);
    // }

    // fputs("Word is cool", vault); // TEST

    // printf("%s\n", file_name);
}