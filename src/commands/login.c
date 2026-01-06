#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "commands.h"

// Private Functions //

void LoginUsername(char *buf, char *file_name, const int BUFFER_SIZE) {
    while (1) {
        printf("Username: ");

        if (ReadInput(buf, BUFFER_SIZE) != 1) {
            fprintf(stderr, RED "byteman input: error: Input error or EOF.\n" TRY_BYTEMAN_HELP RESET);
            exit(1);
        }

        strcpy(file_name, buf);
        strcat(file_name, ".vault");
        
        if (FileExists(file_name)) {
            break;
        }
    }
}

void LoginPassword(char *buf, char *file_name, const int BUFFER_SIZE) {
    FILE *vault = fopen(file_name, "r");
    if (!vault) {
        perror("File opening failed");
        exit(1);
    }

    

    


}

// void LoginPassword(char *buf, const int BUFFER_SIZE)

// Public API //

void Login() {
    char user_name[20 + 1];
    char password[64 + 1];
    char file_name[sizeof(user_name) + 6 + 1];
    LoginUsername(user_name, file_name, sizeof(user_name));
    LoginPassword(password, file_name, sizeof(password));
    printf("Logged in!\n");
}