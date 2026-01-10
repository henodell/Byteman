#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "Cli.h"
#include "Utils.h"

struct GlobalFlags g_flags = {0};

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
            fprintf(stderr, "byteman: unknown option. See \'byteman --help\'.");
            exit(1);
        }
    }

    // Immediate flags
    if (g_flags.help) {
        printf("Usage: byteman [--version] [--verbose] [--help] <command> [<args>]");
        exit(0);
    }

    if (g_flags.version) {
        printf("byteman version 0.1.0");
        exit(0);
    }
}

// Standardises input by making it all lowercase
void ToLowerCase(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = tolower(s[i]);
    }
}

// Public API //

void ParseArgs(int argc, char **argv) {
    // Bounds check
    if (argc < 2) {
        fprintf(stderr, "expected atleast 2 args with a command, got %i args\n", argc);
        exit(1);
    }

    ParseGlobalFlags(argc, argv);
    char *command = argv[optind];
    ToLowerCase(command);

    // create an array with only arguments for command
    char **argv_args = argv + optind + 1;

    CommandArgs args = {argc, argv_args};
    struct CliParams params = {command, &args, &g_flags};

    DoLookup(params);
}