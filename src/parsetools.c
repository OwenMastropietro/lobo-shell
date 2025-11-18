#include "parsetools.h"
#include "constants.h"
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Parse a command line into a list of tokens, separated by whitespace.
 * Returns the number of tokens.
 */
int split_cmd_line(char *line, char *words[]) {
    char *saveptr;
    char *delimiters = " \t\n"; // whitespace

    int i = 0;
    words[i] = strtok_r(line, delimiters, &saveptr);
    while (words[i] != NULL && i < MAX_LINE_TOKENS - 1) {
        i++;
        words[i] = strtok_r(NULL, delimiters, &saveptr);
    };

    return i;
}
