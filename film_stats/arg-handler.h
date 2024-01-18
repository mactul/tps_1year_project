#ifndef STATS_ARG_HANDLER_H
#define STATS_ARG_HANDLER_H

#include <stdbool.h>
#include "src/data_structs.h"

bool parse_args(int argc, char* argv[], StatsArguments* args_structure, int* arg_rest);

#endif