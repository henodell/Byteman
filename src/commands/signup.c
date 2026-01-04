#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "utils.h"

typedef int (*InputCondition)(char *input);

// Private Functions //

int AlnumCheck(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isalnum(str[i])) {
            return 0;
        }
    }
    return 1;
}

int VarietyCheck(char *str) {
// TODO
}

int ConfirmCheck(char *str, char *confirm) {
    if (strcmp(str, confirm) == 0) {
        return 1;
    }

    return 0;
}

void GetInputConfirm(char *msg, char *buf, int buffer_size, char *original) {
    while (1) {
        printf(msg);
        if (fgets(buf, buffer_size, stdin) == NULL) {
            break;
        }

        buf[strcspn(buf, "\n")] = 0;

        if (ConfirmCheck(buf, original) == 1) {
            break;
        }
    }
}

void GetInput(char *msg, char *buf, int buffer_size, InputCondition cond) {
    while (1) {
        printf(msg);
        if (fgets(buf, buffer_size, stdin) == NULL) {
            break;
        }

        buf[strcspn(buf, "\n")] = 0;

        if (cond(buf) == 1) {
            break;
        } 
    }
}


// Public API //

void Signup() {
    char user_name[20 + 1];
    GetInput("Enter a username(20 chars, alphanumeric): ", 
                    user_name, 
                    20 + 1,
                    AlnumCheck);

    // so .vault file doesnt get overwritten
    char file_name[28];
    snprintf(file_name, sizeof(file_name), "%s.vault", user_name);
    if (FileExists(file_name)) {
        fprintf(stderr, RED "byteman file: error: file already exists, use a different username.\n" TRY_BYTEMAN_HELP RESET);
        return;
    }
    
    char password[64 + 1];
    GetInput("Enter a master password(max 64 chars): ", 
                    password, 
                    64 + 1,
                    AlnumCheck); // TODO: replace alnumcheck with varietycheck

    char pass_confirm[64 + 1];
    GetInputConfirm("Confirm your master password: ", 
                    pass_confirm, 
                    64 + 1, 
                    password);

    printf(GREEN "Successfully signed up!\n" RESET);
    FILE *vault = fopen(file_name, "a");
    fputs("Word is cool", vault); // TEST

    // printf("%s\n", file_name);
}