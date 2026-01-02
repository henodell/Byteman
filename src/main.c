#include <stdio.h>
#include "cli.h"

int main(int argc, char **argv) {
    printf("%i\n", argc);
    parseArgs(argc, argv);
}