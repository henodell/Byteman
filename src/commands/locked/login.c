#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <errno.h>
#include "Utils.h"
#include "LockedCommands.h"
#include "Crypto.h"

// Public API //

void Login() {
    printf(GREEN "Successfully Logged in!" RESET);
}