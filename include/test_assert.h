#pragma once
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, ...)                                                      \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "Assertion failed: (%s) %s:%d\n", #cond, __FILE__, \
                    __LINE__);                                                 \
            fprintf(stderr, "Error: ");                                        \
            fprintf(stderr, __VA_ARGS__);                                      \
            fprintf(stderr, "\n\n");                                           \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)
