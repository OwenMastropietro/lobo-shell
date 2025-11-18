#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SHELL_NAME "lsh"

// limits
#define MAX_LINE_CHARS 1024
#define MAX_LINE_TOKENS 100

// special characters
#define PIPE "|"
#define REDIR_IN "<"
#define REDIR_OUT ">"
#define REDIR_APPEND ">>"

// pipe file descriptors
#define READ_END 0
#define WRITE_END 1

#endif // CONSTANTS_H
