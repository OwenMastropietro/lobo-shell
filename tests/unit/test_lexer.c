#include "constants.h"
#include "parsetools.h"
#include "test_assert.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *input;
    const char *expected[MAX_LINE_TOKENS];
    int expected_count;
} TestCase;

static TestCase tests[] = {
    {
        .input = "",
        .expected = {NULL},
        .expected_count = 0,
    },
    {
        .input = "     ",
        .expected = {NULL},
        .expected_count = 0,
    },
    {
        .input = "\t",
        .expected = {NULL},
        .expected_count = 0,
    },
    {
        .input = "\n",
        .expected = {NULL},
        .expected_count = 0,
    },
    {
        .input = "hello",
        .expected = {"hello"},
        .expected_count = 1,
    },
    {
        .input = "hello world",
        .expected = {"hello", "world"},
        .expected_count = 2,
    },
    {
        .input = "   echo   hello   world ",
        .expected = {"echo", "hello", "world"},
        .expected_count = 3,
    },
    {
        .input = "ls -l /home/user",
        .expected = {"ls", "-l", "/home/user"},
        .expected_count = 3,
    },
    {
        .input = "a   b\tc\nd",
        .expected = {"a", "b", "c", "d"},
        .expected_count = 4,
    },
    {
        .input = "echo \"hello world\" !",
        .expected = {"echo", "\"hello", "world\"", "!"},
        .expected_count = 4,
    },
};

int main(void) {
    static const int num_tests = sizeof(tests) / sizeof(TestCase);

    for (int i = 0; i < num_tests; i++) {
        TestCase *tc = &tests[i];

        char line[MAX_LINE_CHARS];
        strncpy(line, tc->input, MAX_LINE_CHARS);
        line[MAX_LINE_CHARS - 1] = '\0';
        assert(strcmp(line, tc->input) == 0);

        TokenStream ts = split_cmd_line(line);
        int num_tokens = ts.size;

        // I'm keeping ASSERT for learning purposes.
        // Otherwise, it isn't yet informative enough to replace std::assert.
        ASSERT(num_tokens == tc->expected_count,
               "expected: %d, got: %d (input: %s)", tc->expected_count,
               num_tokens, tc->input);
        for (int t = 0; t < num_tokens; t++) {
            ASSERT(strcmp(ts.tokens[t].text, tc->expected[t]) == 0,
                   "expected: %s, got: %s (input: %s)", tc->expected[t],
                   ts.tokens[t].text, tc->input);
        }

        free(ts.tokens);
    }

    printf("passed.\n\n");

    return 0;
}
