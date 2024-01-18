#ifndef GUI_ARG_HANDLER_H
#define GUI_ARG_HANDLER_H

#include <stdbool.h>
#include "src/data_structs.h"

bool parse_args(int argc, char* argv[], GuiArguments* args_structure, int* arg_rest);

#endif