#ifndef CALCULATE_STATS_H
#define CALCULATE_STATS_H

#include "src/data_structs.h"

SA_DynamicArray* calculate_all_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const Arguments* filter_options);

#endif