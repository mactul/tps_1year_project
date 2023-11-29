#ifndef PARSER_BIN_H
#define PARSER_BIN_H

#include <SA/memory/dynamic_array.h>
#include "src/data_structs.h"
#include <stdio.h>

SA_DynamicArray* read_all_films(const char* in_filename);

#endif