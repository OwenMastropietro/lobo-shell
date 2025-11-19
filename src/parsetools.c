#include "parsetools.h"
#include "assert.h"
#include "constants.h"
#include "types.h"
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prints the given error message and exits.
void syserror(const char *s, ...) {
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, " (%s)\n", strerror(errno));
    va_end(args);
    exit(EXIT_FAILURE);
}

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

static char *skip_ws(char *p) {
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

static char *scan_quoted(char *start) {
    char *p = start;
    while (*p) {
        if (*p == '"' && *(p - 1) != '\\') {
            return p;
        }
        p++;
    }
    return p;
}

static char *scan_unquoted(char *start) {
    char *p = start;
    while (*p && !isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

TokenType get_operator(const char *s, int *len) {
    if (*s == '|') {
        *len = 1;
        return T_PIPE;
    }
    if (*s == '<') {
        *len = 1;
        return T_REDIR_IN;
    }
    if (*s == '>') {
        if (*(s + 1) == '>') {
            *len = 2;
            return T_REDIR_APPEND;
        }
        *len = 1;
        return T_REDIR_OUT;
    }

    *len = 0;
    return T_WORD;
}

/**
 * Returns a stream of tokens by splitting cmd line input on whitespace;
 * preserves quoted sections.
 */
TokenStream split_cmd_line(char *line) {
    TokenStream ts = {.tokens = NULL, .size = 0};
    if (!line) {
        return ts;
    }
    if (strlen(line) > MAX_LINE_CHARS) {
        syserror(SHELL_NAME ": command too long.");
        return ts;
    }

    ts.tokens = calloc(MAX_LINE_TOKENS, sizeof(Token));
    if (!ts.tokens) {
        return ts;
    }

    char *p = line;

    while (*p) {
        p = skip_ws(p);
        if (*p == '\0') {
            break;
        }

        int op_len = 0; // fiiw
        TokenType op_type = get_operator(p, &op_len);
        if (op_type != T_WORD) {
            char *op_text = strndup(p, op_len);
            ts.tokens[ts.size++] = (Token){
                .type = op_type,
                .text = op_text,
            };
            p += op_len;
            continue;
        }

        char *tok_start;
        char *tok_end;

        if (*p == '"') {
            p++;
            tok_start = p;
            tok_end = scan_quoted(tok_start);

            if (*tok_end == '"') {
                *tok_end = '\0';
                ts.tokens[ts.size++] = (Token){
                    .type = T_WORD,
                    .text = tok_start,
                };
                p = tok_end + 1;
            } else {
                ts.tokens[ts.size++] = (Token){
                    .type = T_WORD,
                    .text = tok_start,
                };
                break;
            }

        } else {
            tok_start = p;
            tok_end = scan_unquoted(tok_start);

            if (*tok_end) {
                *tok_end = '\0';
                ts.tokens[ts.size++] = (Token){
                    .type = T_WORD,
                    .text = tok_start,
                };
                p = tok_end + 1;
            } else {
                ts.tokens[ts.size++] = (Token){
                    .type = T_WORD,
                    .text = tok_start,
                };
                break;
            }
        }
    }

    return ts;
}
