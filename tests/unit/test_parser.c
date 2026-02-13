#include "constants.h"
#include "parsetools.h"
#include "test_assert.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    TokenStream input;
    Pipeline exp;
} TestCase;

static TestCase tests[] = {
    { // ""
        .input = {.tokens = NULL, .size = 0},
        .exp = {.cmds = NULL, .size = 0},
    }, { // "hello"
        .input = (TokenStream){
            .tokens = (Token[]){{
                .type = T_WORD,
                .text = "hello"
            }},
            .size = 1,
        },
        .exp = {
            .cmds = (Command[]){{
                .argv = (char *[]){"hello", NULL},
                .argc = 1,
                .redirs = NULL,
                .num_redirs = 0,
            }},
            .size = 1,
        },
    }, { // "hello world"
        .input = (TokenStream){
            .tokens = (Token[]){{
                .type = T_WORD,
                .text = "hello",
            }, {
                .type = T_WORD,
                .text = "world",
            }},
            .size = 2,
        },
        .exp = {
            .cmds = (Command[]){{
                .argv = (char *[]){"hello", "world", NULL},
                .argc = 2,
                .redirs = NULL,
                .num_redirs = 0,
            }},
            .size = 1,
        },
    },
    {
        // "ls -l /home/user",
        .input = (TokenStream){
            .tokens = (Token[]){{
                .type = T_WORD,
                .text = "ls",
            }, {
                .type = T_WORD,
                .text = "-l",
            }, {
                .type = T_WORD,
                .text = "/home/user",
            }},
            .size = 3,
        },
        .exp = {
            .cmds = (Command[]){{
                .argv = (char *[]){"ls", "-l", "/home/user", NULL},
                .argc = 3,
                .redirs = NULL,
                .num_redirs = 0,
            }},
            .size = 1,
        },
    },
    {
        // "ls /usr/bin | head -10",
        .input = (TokenStream){
            .tokens = (Token[]){{
                .type = T_WORD,
                .text = "ls",
            }, {
                .type = T_WORD,
                .text = "/usr/bin",
            }, {
                .type = T_PIPE, .text = "|",
            }, {
                .type = T_WORD,
                .text = "head",
            }, {
                .type = T_WORD,
                .text = "-10",
            }},
            .size = 5,
        },
        .exp = {
            .cmds = (Command[]){{
                .argv = (char *[]){"ls", "/usr/bin", NULL},
                .argc = 2,
                .redirs = NULL,
                .num_redirs = 0,
            }, {
                .argv = (char *[]){"head", "-10", NULL},
                .argc = 2,
                .redirs = NULL,
                .num_redirs = 0,
            }},
            .size = 2,
        },
    },
    {
        // "grep foo < in.txt | sort > out.txt | uniq >> append.txt",
        .input = (TokenStream){
            .tokens = (Token[]){{
                .type = T_WORD,
                .text = "grep",
            }, {
                .type = T_WORD,
                .text = "foo",
            }, {
                .type = T_REDIR_IN,
                .text = "<",
            }, {
                .type = T_WORD,
                .text = "in.txt",
            }, {
                .type = T_PIPE,
                .text = "|",
            }, {
                .type = T_WORD,
                .text = "sort",
            }, {
                .type = T_REDIR_OUT,
                .text = ">",
            }, {
                .type = T_WORD,
                .text = "out.txt",
            }, {
                .type = T_PIPE,
                .text = "|",
            }, {
                .type = T_WORD,
                .text = "uniq",
            }, {
                .type = T_REDIR_APPEND,
                .text = ">>",
            }, {
                .type = T_WORD,
                .text = "append.txt",
            }},
            .size = 12,
        },
        .exp = {
            .cmds = (Command[]){{
                // grep foo < in.txt
                .argv = (char *[]){"grep", "foo", NULL},
                .argc = 2,
                .redirs = (Redir[]){{
                    .type=T_REDIR_IN,
                    .filename = "in.txt",
                }},
                .num_redirs = 1,
            }, {
                // sort > out.txt
                .argv = (char *[]){"sort", NULL},
                .argc = 1,
                .redirs = (Redir[]){{
                    .type = T_REDIR_OUT,
                    .filename = "out.txt",
                }},
                .num_redirs = 1,
            }, {
                // uniq >> append.txt
                .argv = (char *[]){"uniq", NULL},
                .argc = 1,
                .redirs = (Redir[]){{
                    .type = T_REDIR_APPEND,
                    .filename = "append.txt" ,
                }},
                .num_redirs = 1,
            }},
            .size = 3,
        },
    },
};

void test_parser(void) {
    static const int num_tests = sizeof(tests) / sizeof(TestCase);

    for (int i = 0; i < num_tests; ++i) {
        Pipeline res = parser(tests[i].input);
        Pipeline exp = tests[i].exp;

        assert(res.size == exp.size);

        for (int j = 0; j < res.size; ++j) {
            assert(res.cmds[j].argc == exp.cmds[j].argc);
            assert(res.cmds[j].num_redirs == exp.cmds[j].num_redirs);
            for (int k = 0; k < res.cmds[k].argc; ++k) {
                assert(strcmp(res.cmds[j].argv[k], exp.cmds[j].argv[k]) == 0);
            }
            for (int k = 0; k < res.cmds[j].num_redirs; ++k) {
                assert(res.cmds[j].redirs[k].type ==
                       exp.cmds[j].redirs[k].type);
                assert(strcmp(res.cmds[j].redirs[k].filename,
                              exp.cmds[j].redirs[k].filename) == 0);
            }
        }

        free_pipeline(&res);
    }

    printf("  lexer - passed\n");
}
