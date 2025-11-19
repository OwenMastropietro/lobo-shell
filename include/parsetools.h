#pragma once
#include "types.h"

char *read_line(void);
TokenStream split_cmd_line(char *line);
