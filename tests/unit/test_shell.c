#include <stdio.h>

void test_lexer(void);
void test_parser(void);

int main(void) {
    printf("Running unit tests...\n");

    test_lexer();
    test_parser();

    printf("Passed\n");

    return 0;
}
