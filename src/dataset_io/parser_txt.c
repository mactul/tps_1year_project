#include <stdio.h>
#include <inttypes.h>
#include <SA/memory/dynamic_array.h>
#include "src/dataset_io/parser_txt.h"

static int compare_ratings(const void* el1, const void* el2)
{
    return ((Rating*)el1)->user_id - ((Rating*)el2)->user_id;
}

int read_movie_file(SA_DynamicArray* films, const char* filename)
{
    int error_code = 0;
    
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }

    Film film = {.rating_count = 0, .sum_rating = 0};

    fscanf(file, "%" PRIu32 ":", &(film.film_id));

    SA_DynamicArray* ratings = SA_dynarray_create_size_hint(Rating, EXPECTED_RATINGS_PER_FILM_NUMBER);
    Rating rating;
    uint16_t year;
    
    while (fscanf(file, "%" SCNu32 ",%" SCNu8 ",%" SCNu16 "-%" SCNu8 "-%" SCNu8, &(rating.user_id), &(rating.note), &year, &(rating.month), &(rating.day)) != EOF)
    {
        rating.offseted_year = year - YEARS_OFFSET;
        SA_dynarray_append(Rating, ratings, rating);
        film.rating_count++;
        film.sum_rating += rating.note;
    }

    SA_dynarray_qsort(ratings, compare_ratings);

    film.ratings = ratings;

    SA_dynarray_append(Film, films, film);

QUIT:
    return error_code;
}