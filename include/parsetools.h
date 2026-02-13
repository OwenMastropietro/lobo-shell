#pragma once
#include "types.h"

char *read_line(void);

TokenStream split_cmd_line(char *line);
void print_tokens(const TokenStream *ts);

Pipeline parse_tokens(TokenStream ts);
void print_pipeline(const Pipeline *pl);
void free_pipeline(Pipeline *pl);
