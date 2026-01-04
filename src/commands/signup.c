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

}

int ConfirmCheck(char *str) {

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
    char user_msg[] = "Enter a username(20 chars, alphanumeric): ";
    char user_name[20+1];
    GetInput(user_msg, user_name, 20 + 1, AlnumCheck);

    char password_msg[] = "Enter a master password(max 64 chars): ";
    char password[64 + 1];
    GetInput(password_msg, password, 64 + 1, VarietyCheck);

    char pass_confirm_msg[] = "Confirm your master password: ";
    char pass_confirm[64 + 1];
    GetInput(password_msg, password, 64 + 1, ConfirmCheck);
}