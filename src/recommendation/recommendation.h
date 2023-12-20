#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "src/data_structs.h"

double distance_between_films(Film* film1, Film* film2, const SA_DynamicArray* reviewers);
double calculate_recommendation(const Film* film, const SA_DynamicArray* liked_films, double avg_film_note, const SA_DynamicArray* reviewers);

#endif