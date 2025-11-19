/**
 * Implements a unix shell according to the following structure:
 * <input> -> lexer -> <token-stream> -> parser <pipeline>
 *
 * Beautiful baseline:
 * https://brennan.io/2015/01/16/write-a-shell-in-c/
 */

#include "constants.h"
#include "parsetools.h"
#include "types.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void shell_prompt(bool interactive) {
    if (interactive) {
        printf("%s> ", SHELL_NAME);
        fflush(stdout);
    }
}

void shell_loop(void) {
    bool interactive = isatty(STDERR_FILENO);

    char *line;
    TokenStream ts;
    bool running = true;

    while (running) {
        shell_prompt(interactive);

        line = read_line();
        ts = split_cmd_line(line);
        // running = shell_execute(tokens);

        for (int i = 0; i < ts.size; i++) {
            Token t = ts.tokens[i];
            printf("(Token){.type=%s, .text=%s}\n", tok_name(t.type), t.text);
        }

        free(line);
        free(ts.tokens);
    }
}

int main(void) {
    // Load config files, if any.

    // Run command loop.
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
