#ifndef MIN_REVIEWS_H
#define MIN_REVIEWS_H

#include "src/data_structs.h"

void filter_min_reviews(Film* film_filtered, const Film* film_to_filter, const SA_DynamicArray* reviewers, uint32_t min);

#endif