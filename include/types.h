#pragma once
#include <stdbool.h>

typedef enum TokenType {
    T_WORD,
    T_PIPE,
    T_REDIR_IN,
    T_REDIR_OUT,
    T_REDIR_APPEND
} TokenType;

static inline const char *tok_name(TokenType t) {
    switch (t) {
        case T_WORD:            return "T_WORD";
        case T_PIPE:            return "T_PIPE";
        case T_REDIR_IN:        return "T_REDIR_IN";
        case T_REDIR_OUT:       return "T_REDIR_OUT";
        case T_REDIR_APPEND:    return "T_REDIR_APPEND";
        default:                return "UNKNOWN";
    }
}

typedef struct Token {
    TokenType type;
    char *text; // todo: rename? lexeme? literal?
} Token;

typedef struct TokenStream {
    Token *tokens;
    int size;
} TokenStream;

typedef struct Command {
    char **argv;
    char *infile;
    char *outfile;
    bool append;
    bool is_first;
    bool is_last;
} Command;

typedef struct Pipeline {
    Command *cmds;
    int size;
} Pipeline;
