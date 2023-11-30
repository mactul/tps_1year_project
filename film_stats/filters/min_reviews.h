#ifndef MIN_REVIEWS_H
#define MIN_REVIEWS_H

#include "src/data_structs.h"

Film* filter_min_reviews(const Film* film, SA_DynamicArray* reviewers, uint32_t min);

#endif