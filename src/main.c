#include <stdio.h>
#include "cli.h"
// #include "utils.h"

int main(int argc, char **argv) {
    printf("%i\n", argc);
    parse(argc, argv);
}