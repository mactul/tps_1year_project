#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "src/data_structs.h"

double distance_between_films(Film* film1, Film* film2, SA_DynamicArray* reviewers);

#endif