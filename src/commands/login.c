#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "commands.h"

// Private Functions //

void AlphaNumericCheck(char *s, int *check_bool) {
    *check_bool = 1; // assume valid

    for (int i = 0; s[i] != 0; i++) {
        if (!isalnum(s[i])) {
            *check_bool = 0;
            break;
        }
    }
}

// void TrimSpaces(char *s) {
//     // loop from end to beginning until reaching normal char
//     for (int i = strlen(s) - 1; i >= 0; i--) {
//         if (isalpha(s[i])) {
//             break;
//         } else {
//             s[i] = 0;
//         };
//     }

//     // loop from beginning to end until normal char
//     for (int i = 0; i > strlen(s); i++) {
//         if (isalpha(s[i])) {
//             break;
//         } else {
//             s[i] = 0;
//         }
//     }

//     printf("%s\n", s);
// }

// Public API //

void Login() {
    char user_name[20 + 1];
    int pass_check = 1;

    do {
        printf("Username (max length: 20, alphanumeric): ");
        if (fgets(user_name, sizeof(user_name), stdin) == NULL) {
            fprintf(stderr, RED "byteman input: error: %s\n" TRY_BYTEMAN_HELP RESET, strerror(errno));
            break;
        }

        // remove newline for alpha check to not break
        user_name[strcspn(user_name, "\n")] = 0;

        // trimSpaces(username);
        AlphaNumericCheck(user_name, &pass_check);
    } while (pass_check == 0);

    printf("Username is %s", user_name);
}