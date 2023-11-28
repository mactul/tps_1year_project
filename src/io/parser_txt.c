#include <stdio.h>
#include <inttypes.h>
#include <SA/memory/dynamic_array.h>
#include "src/io/parser_txt.h"

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

    SA_DynamicArray* ratings = SA_dynarray_create(Rating);
    Rating rating;
    uint16_t year;
    
    while (fscanf(file, "%" SCNu32 ",%" SCNu8 ",%" SCNu16 "-%" SCNu8 "-%" SCNu8, &(rating.user_id), &(rating.note), &year, &(rating.month), &(rating.day)) != EOF)
    {
        rating.offseted_year = year - YEARS_OFFSET;
        SA_dynarray_append(Rating, ratings, rating);
        film.rating_count++;
        film.sum_rating += rating.note;
    }

    film.ratings = ratings;

    SA_dynarray_append(Film, films, film);

QUIT:
    return error_code;
}