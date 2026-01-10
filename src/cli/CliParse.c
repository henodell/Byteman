#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "Cli.h"
#include "Utils.h"

// Private Functions //

struct GlobalFlags g_flags;

const struct option long_options[] = {
    {"version", no_argument, &g_flags.version, 1},
    {"verbose", no_argument, &g_flags.verbose, 1},
    {"help", no_argument, &g_flags.help, 1},
    {0, 0, 0, 0}
};

// Parses all global flags
void ParseGlobalFlags(int argc, char **argv) {

    int long_index; 
    int opt;

    while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1) {
        if (opt == '?') {
            fprintf(stderr, "Unknown option. try byteman --help\n");
            exit(1);
        }
    }

    // Immediate flags
    if (g_flags.help) {
        printf("Usage: byteman [--verbose] [--version] [--help] <command> <command-flags>");
        exit(0);
    }

    if (g_flags.version) {
        printf("byteman version 0.1.0");
        exit(0);
    }

    if (g_flags.verbose) {
        printf("Verbose mode enabled\n");
        return;
    }
}

//  byteman [global-flags] <command> [command-flags]

// Standardises input by making it all lowercase
void ToLowerCase(char *s) {
    for (int i = 0; i < strlen(s); i++) {
        s[i] = tolower(s[i]);
    }
}

// Public API //

void ParseArgs(int argc, char **argv) {
    // Bounds check
    if (argc > 5 || argc < 2) {
        fprintf(stderr, RED "byteman parse: error: expected 2-5 args got %d\n" TRY_BYTEMAN_HELP RESET, argc);
        exit(1);
    }

    ParseGlobalFlags(argc, argv);
    char *command = argv[optind];
    ToLowerCase(command);

    CommandArgs args = {argc, argv};
    struct CliParams params = {command, &args, &g_flags};

    DoLookup(params);
}