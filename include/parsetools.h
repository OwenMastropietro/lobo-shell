#pragma once
#include "types.h"

char *read_line(void);

TokenStream lexer(char *line);
void print_tokens(const TokenStream *ts);

Pipeline parser(TokenStream ts);
void print_pipeline(const Pipeline *pl);
void free_pipeline(Pipeline *pl);
