#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <SA/memory/dynamic_array.h>

#define YEARS_OFFSET 1970

typedef struct _film {
    uint32_t film_id;
    uint32_t rating_count;
    uint64_t sum_rating;
    SA_DynamicArray* ratings;
} Film;

typedef struct _rating {
    uint32_t user_id;
    uint8_t note;
    uint8_t day;
    uint8_t month;
    uint8_t offseted_year;
} Rating;

void films_list_free(SA_DynamicArray** films);

#endif