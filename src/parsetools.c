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

static void cmd_add_arg(Command *cmd, int *cap, const char *arg) {
    if (cmd->argc + 1 >= *cap) {
        *cap *= 2;
        cmd->argv = realloc(cmd->argv, sizeof(char *) * (*cap));
        assert(cmd->argv);
    }

    cmd->argv[cmd->argc++] = strdup(arg);
}

static void cmd_add_redir(Command *cmd, int *cap, TokenType type, const char *filename) {
    if (cmd->num_redirs + 1 >= *cap) {
        *cap *= 2;
        cmd->redirs = realloc(cmd->redirs, sizeof(Redir) * (*cap));
        assert(cmd->redirs);
    }

    cmd->redirs[cmd->num_redirs++] = (Redir){
        .type = type,
        .filename = strdup(filename),
    };
}

static Command parse_command(TokenStream *ts, int *i) {
    int argv_cap = 8;  // todo
    int redir_cap = 4; // todo
    Command cmd = {
        .argv = malloc(sizeof(char *) * argv_cap),
        .argc = 0,
        .redirs = malloc(sizeof(Redir) * redir_cap),
        .num_redirs = 0,
    };
    assert(cmd.argv);
    assert(cmd.redirs);

    while (*i < ts->size) {
        Token token = ts->tokens[*i];

        if (token.type == T_PIPE) {
            break;
        }

        // Handle Redirects.
        if (token.type == T_REDIR_IN || token.type == T_REDIR_OUT ||
            token.type == T_REDIR_APPEND) {

            if (*i + 1 >= ts->size || ts->tokens[*i + 1].type != T_WORD) {
                syserror(SHELL_NAME
                         ": Syntax error: expected filename after %s\n",
                         tok_name(token.type));
            }

            char *filename = ts->tokens[*i + 1].text;
            cmd_add_redir(&cmd, &redir_cap, token.type, filename);
            *i += 2;
            continue;
        }

        // Handle Normal Args.
        if (token.type == T_WORD) {
            cmd_add_arg(&cmd, &argv_cap, token.text);
            (*i)++;
            continue;
        }

        (*i)++; // skip unexpected token
        // syserror(SHELL_NAME "Unexpected token: %s\n", tok_name(token->type));
    }

    // if (cmd->argc >= argv_cap) { // or jsut -1 in other ops
    //     cmd->argv = realloc(cmd->argv, sizeof(char *) * (argv_cap + 1));
    // }
    // cmd->argv[cmd->argc] = NULL; // sentinel

    // if (*i < ts->size && ts->tokens[*i].type == T_PIPE) {
    //     (*i)++; // skip pipe
    // }

    cmd.argv[cmd.argc] = NULL;
    return cmd;
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
 *
 * Splits a line into whitespace-delimited tokens, preserving quoted sequences
 * as single tokens.
 *
 * Example:
 * line:    echo "hello world" | wc -l
 * tokens:  [echo, "hello world", |, wc, -l]
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

void print_tokens(const TokenStream *ts) {
    for (int i = 0; i < ts->size; ++i) {
        Token t = ts->tokens[i];
        printf("(Token){.type=%s, .text=%s}\n", tok_name(t.type), t.text);
    }
}

Pipeline parse_tokens(TokenStream ts) {
    int cap = 4; // todo

    Pipeline pl = {
        .cmds = malloc(sizeof(Command) * cap),
        .size = 0,
    };
    assert(pl.cmds);

    int i = 0;
    while (i < ts.size) {
        if (pl.size >= cap) {
            cap *= 2;
            pl.cmds = realloc(pl.cmds, sizeof(Command) * cap);
            assert(pl.cmds);
        }

        pl.cmds[pl.size++] = parse_command(&ts, &i);

        if (i < ts.size && ts.tokens[i].type == T_PIPE) {
            i++;
        }
    }

    return pl;
}

void print_pipeline(const Pipeline *pl) {
    if (!pl) {
        printf("\nEmpty Pipeline.\n");
        return;
    }

    printf("\nPipeline (size=%d):\n", pl->size);
    for (int i = 0; i < pl->size; ++i) {
        Command *cmd = &pl->cmds[i];

        printf("Command %d:\n", i);

        printf("  argv = [");
        for (int j = 0; j < cmd->argc; ++j) {
            printf("\"%s\"", cmd->argv[j]);
            if (j + 1 < cmd->argc) {
                printf(", ");
            }
        }
        printf("]\n");

        printf("  redirs = [");
        for (int j = 0; j < cmd->num_redirs; ++j) {
            Redir *rd = &cmd->redirs[j];
            printf("{%s, %s}", tok_name(rd->type), rd->filename);
            if (j + 1 < cmd->num_redirs) {
                printf(", ");
            }
        }
        printf("]\n");

        printf("\n");
    }
}

void free_pipeline(Pipeline *pl) {
    if (!pl) {
        return;
    }

    for (int i = 0; i < pl->size; ++i) {
        for (int j = 0; j < pl->cmds[i].argc; ++j) {
            free(pl->cmds[i].argv[j]);
        }
        free(pl->cmds[i].argv);

        for (int j = 0; j < pl->cmds[i].num_redirs; ++j) {
            free(pl->cmds[i].redirs[j].filename);
        }
        free(pl->cmds[i].redirs);
    }

    free(pl->cmds);
}
