#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <SA/memory/dynamic_array.h>

#define NUMBER_OF_YEARS_LOGGED_IN_STATS 10

#define YEARS_OFFSET 1970

#define EXPECTED_FILM_NUMBERS 18000
#define EXPECTED_RATINGS_PER_FILM_NUMBER 256

typedef struct _film {
    uint32_t film_id;
    uint32_t rating_count;
    SA_DynamicArray* ratings;
} Film;

typedef struct _rating {
    uint32_t user_id;
    uint8_t note;
    uint8_t day;
    uint8_t month;
    uint8_t offseted_year;
} Rating;

typedef struct _arguments {
    uint32_t limit;
    uint32_t film_id;
    uint32_t min_reviews;
    uint32_t timeout_milli;
    const char* output_folder;
    const char* only_reviewers;
    const char* bad_reviewers;
} Arguments;

typedef struct _film_stats {
    uint32_t film_id;
    uint32_t kept_rating_count;
    float mean_rating;
    float median_rating;
    float standard_deviation;
    float recommendation;
    float mean_rating_over_years[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    uint32_t kept_rating_count_over_years[NUMBER_OF_YEARS_LOGGED_IN_STATS];
} FilmStats;

void films_list_free(SA_DynamicArray** films);

#endif