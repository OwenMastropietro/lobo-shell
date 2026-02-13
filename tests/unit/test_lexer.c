#include "constants.h"
#include "parsetools.h"
#include "test_assert.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *input;
    TokenStream exp;
} TestCase;

static TestCase tests[] = {
    {
        .input = "",
        .exp = {.tokens = NULL, .size = 0},
    },
    {
        .input = "     ",
        .exp = {.tokens = NULL, .size = 0},
    },
    {
        .input = "\t",
        .exp = {.tokens = NULL, .size = 0},
    },
    {
        .input = "\n",
        .exp = {.tokens = NULL, .size = 0},
    },
    {
        .input = "hello",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="hello"},
            },
            .size = 1
        },
    },
    {
        .input = "hello world",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="hello"},
                {.type=T_WORD, .text="world"},
            },
            .size = 2,
        },
    },
    {
        .input = "   echo   hello   world ",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="echo"},
                {.type=T_WORD, .text="hello"},
                {.type=T_WORD, .text="world"},
            },
            .size = 3,
        },
    },
    {
        .input = "ls -l /home/user",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="ls"},
                {.type=T_WORD, .text="-l"},
                {.type=T_WORD, .text="/home/user"},
            },
            .size = 3,
        },
    },
    {
        .input = "a   b\tc\nd",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="a"},
                {.type=T_WORD, .text="b"},
                {.type=T_WORD, .text="c"},
                {.type=T_WORD, .text="d"},
            },
            .size = 4,
        },
        
    },
    {
        .input = "echo \"hello world\" !",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="echo"},
                {.type=T_WORD, .text="hello world"},
                {.type=T_WORD, .text="!"},
            },
            .size = 3,
        },
    },
    {
        .input = "ls /usr/bin | head -10",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="ls"},
                {.type=T_WORD, .text="/usr/bin"},
                {.type=T_PIPE, .text="|"},
                {.type=T_WORD, .text="head"},
                {.type=T_WORD, .text="-10"},
            },
            .size = 5,
        },
    },
    {
        .input = "grep foo < in.txt | sort > out.txt | uniq >> append.txt",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="grep"},
                {.type=T_WORD, .text="foo"},
                {.type=T_REDIR_IN, .text="<"},
                {.type=T_WORD, .text="in.txt"},
                {.type=T_PIPE, .text="|"},
                {.type=T_WORD, .text="sort"},
                {.type=T_REDIR_OUT, .text=">"},
                {.type=T_WORD, .text="out.txt"},
                {.type=T_PIPE, .text="|"},
                {.type=T_WORD, .text="uniq"},
                {.type=T_REDIR_APPEND, .text=">>"},
                {.type=T_WORD, .text="append.txt"},
            },
            .size = 12,
        },
    },
    {
        .input = "echo \"hello \\\"world\\\"\" | grep \"foo bar\" > out.txt",
        .exp = (TokenStream){
            .tokens = (Token[]){
                {.type=T_WORD, .text="echo"},
                {.type=T_WORD, .text="hello \\\"world\\\""},
                {.type=T_PIPE, .text="|"},
                {.type=T_WORD, .text="grep"},
                {.type=T_WORD, .text="foo bar"},
                {.type=T_REDIR_OUT, .text=">"},
                {.type=T_WORD, .text="out.txt"},
            },
            .size = 7,
        },
    },
};

void test_lexer(void) {
    static const int num_tests = sizeof(tests) / sizeof(TestCase);

    for (int i = 0; i < num_tests; i++) {
        TestCase *tc = &tests[i];

        char line[MAX_LINE_CHARS];
        strncpy(line, tc->input, MAX_LINE_CHARS);
        line[MAX_LINE_CHARS - 1] = '\0';
        assert(strcmp(line, tc->input) == 0);

        TokenStream res = split_cmd_line(line);
        TokenStream exp = tc->exp;

        // I'm keeping ASSERT for learning purposes.
        // Otherwise, it isn't yet informative enough to replace std::assert.
        ASSERT(res.size == exp.size, "expected: %d, got: %d (input: %s)",
            exp.size, res.size, tc->input);
        for (int t = 0; t < res.size; t++) {
            ASSERT(strcmp(res.tokens[t].text, exp.tokens[t].text) == 0,
                "expected: %s, got: %s (input: %s)", exp.tokens[t].text,
                res.tokens[t].text, tc->input);
        }

        free(res.tokens);
    }

    printf("  parser - passed\n");
}
