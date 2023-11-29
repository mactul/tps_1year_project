#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include <stdbool.h>
#include "src/data_structs.h"

bool parse_args(int argc, char* argv[], Arguments* args_structure, int* arg_rest);

#endif