#include "constants.h"
#include "parsetools.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char line[MAX_LINE_CHARS];
    char *tokens[MAX_LINE_TOKENS + 1];

    // Loop until user hits Ctrl-D or some other input error occurs.
    while (fgets(line, MAX_LINE_CHARS, stdin)) {
        int num_tokens = split_cmd_line(line, tokens);

        for (int i = 0; i < num_tokens; i++) {
            printf("%s\n", tokens[i]);
        }
    }

    return 0;
}
