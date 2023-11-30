#ifndef PARSER_BIN_H
#define PARSER_BIN_H

#include "src/data_structs.h"

SA_DynamicArray* read_all_reviewers(FILE* file);
SA_DynamicArray* read_all_films(FILE* file);

#endif