#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "commands.h"

// Private Functions //

void alphanumeric_check(char *str, int *check_bool) {
    *check_bool = 1; // assume valid

    for (int i = 0; str[i] != 0; i++) {
        if (!isalnum(str[i])) {
            *check_bool = 0;
            break;
        }
    }
}

void trim_sapces(char *str) {
    // loop from end to beginning until reaching normal char
    for (int i = strlen(str) - 1; i >= 0; i--) {
        if (isalpha(str[i])) {
            break;
        } else {
            str[i] = 0;
        };
    }

    // loop from beginning to end until normal char
    for (int i = 0; i > strlen(str); i++) {
        if (isalpha(str[i])) {
            break;
        } else {
            str[i] = 0;
        }
    }

    printf("%s\n", str);
}

// Public API //

void login() {
    char username[20 + 1];
    int pass_check = 1;

    do {
        printf("Username (max length: 20, alphanumeric): ");
        if (fgets(username, sizeof(username), stdin) == NULL) {
            fprintf(stderr, RED "byteman input: error: %s\n" TRY_BYTEMAN_HELP RESET, strerror(errno));
            break;
        }

        // remove newline for alpha check to not break
        username[strcspn(username, "\n")] = 0;

        // trimSpaces(username);
        alphanumeric_check(username, &pass_check);
    } while (pass_check == 0);

    printf("Username is %s", username);
}