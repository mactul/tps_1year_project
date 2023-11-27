#ifndef MOVIE_RATING_H
#define MOVIE_RATING_H

#include <stdint.h>

typedef struct {
    uint64_t film_id;
    uint64_t rating_count;
    double average_rating;
} Movie;

#endif