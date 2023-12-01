#ifndef PARSER_STATS_H
#define PARSER_STATS_H

#include <stdio.h>
#include <SA/memory/dynamic_array.h>

SA_DynamicArray* read_stats(FILE* file);

#endif