#include "parsetools.h"
#include "constants.h"
#include "types.h"
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns a buffer of input read from the command line.
char *read_line(void) {
    int bufsize = MAX_LINE_CHARS;
    int pos = 0;
    int c;
    char *buffer = malloc(sizeof(char) * bufsize);
    if (!buffer) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    c = getchar();
    while (c != EOF && c != '\n') {
        buffer[pos++] = c;
        if (pos >= bufsize) {
            bufsize += MAX_LINE_CHARS;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                perror("realloc failed");
                exit(EXIT_FAILURE);
            }
        }
        c = getchar();
    }

    buffer[pos] = '\0';
    return buffer;
}

/**
 * Parse a command line into a list of tokens, separated by whitespace.
 * Returns the number of tokens.
 */
TokenStream split_cmd_line(char *line) {
    TokenStream ts = {.tokens = NULL, .size = 0};
    if (!line) {
        return ts;
    }

    ts.tokens = calloc(MAX_LINE_TOKENS, sizeof(Token));
    if (!ts.tokens) {
        return ts;
    }

    char *saveptr;
    char *delimiters = " \t\n"; // whitespace

    int i = 0;
    char *lex = strtok_r(line, delimiters, &saveptr);
    while (lex && i < MAX_LINE_TOKENS) {
        ts.tokens[i].text = lex;
        ts.tokens[i].type = T_WORD;
        i++;

        lex = strtok_r(NULL, delimiters, &saveptr);
    }

    ts.size = i;
    return ts;
}
