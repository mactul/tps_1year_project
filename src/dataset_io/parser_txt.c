#include <stdio.h>
#include <inttypes.h>
#include <SA/memory/dynamic_array.h>
#include <SA/strings/strings.h>
#include "src/dataset_io/parser_txt.h"

#define BUFFER_SIZE 256


static int compare_ratings(const void* el1, const void* el2)
{
    return ((Rating*)el1)->user_id - ((Rating*)el2)->user_id;
}


static uint64_t next_number(char** line_to_destroy)
{
    uint64_t result;
    int i = 0;
    while(SA_CHAR_IS_DIGIT((*line_to_destroy)[i]))
    {
        i++;
    }
    (*line_to_destroy)[i] = '\0';

    result = SA_str_to_uint64(*line_to_destroy);

    *line_to_destroy += i+1;

    return result;
}

static void parse_rating_line(char* line_to_destroy, Rating* rating)
{
    uint16_t year;

    rating->user_id = next_number(&line_to_destroy);
    rating->note = next_number(&line_to_destroy);
    year = next_number(&line_to_destroy);
    rating->offseted_year = year - YEARS_OFFSET;
    rating->month = next_number(&line_to_destroy);
    rating->day = next_number(&line_to_destroy);
}

static uint32_t parse_movie_line(char* line_to_destroy, FilmInfo* film_info)
{
    uint32_t film_id = next_number(&line_to_destroy);
    int i = 0;
    if(SA_startswith_case_unsensitive(line_to_destroy, "NULL"))
    {
        film_info->year = 0;
        while(*line_to_destroy != ',')
        {
            line_to_destroy++;
        }
        line_to_destroy++;
    }
    else
    {
        film_info->year = next_number(&line_to_destroy);
    }
    while(i < MAX_FILM_NAME_SIZE-1 && *line_to_destroy != '\n' && *line_to_destroy != '\r' && *line_to_destroy != '\0')
    {
        film_info->name[i] = *line_to_destroy;
        i++;
        line_to_destroy++;
    }
    film_info->name[i] = '\0';

    return film_id;
}

int read_movie_file(SA_DynamicArray* films, const char* filename)
{
    int i = 0;
    int error_code = 0;
    Rating rating;
    char buffer[BUFFER_SIZE];
    Film film = {.rating_count = 0};
    SA_DynamicArray* ratings = NULL;
    FILE* file = fopen(filename, "r");

    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }

    fgets(buffer, BUFFER_SIZE, file);

    while(SA_CHAR_IS_DIGIT(buffer[i]))
    {
        i++;
    }
    buffer[i] = '\0';

    film.film_id = SA_str_to_uint64(buffer);

    ratings = SA_dynarray_create_size_hint(Rating, EXPECTED_RATINGS_PER_FILM_NUMBER);
    
    while (fgets(buffer, BUFFER_SIZE, file) != NULL)
    {
        parse_rating_line(buffer, &rating);
        SA_dynarray_append(Rating, ratings, rating);
        film.rating_count++;
    }

    SA_dynarray_qsort(ratings, compare_ratings);

    film.ratings = ratings;

    SA_dynarray_append(Film, films, film);

QUIT:
    fclose(file);
    return error_code;
}

SA_DynamicArray* get_films_infos(const char* movie_titles_filepath)
{
    char buffer[BUFFER_SIZE];
    SA_DynamicArray* films_infos = NULL;
    FILE* file = fopen(movie_titles_filepath, "r");

    if(file == NULL)
    {
        return NULL;
    }

    films_infos = SA_dynarray_create_size_hint(FilmInfo, EXPECTED_FILM_NUMBERS);

    while (fgets(buffer, BUFFER_SIZE, file) != NULL)
    {
        FilmInfo film_info;
        uint32_t film_id = parse_movie_line(buffer, &film_info);
        SA_dynarray_set(FilmInfo, films_infos, film_id, film_info);
    }

    fclose(file);

    return films_infos;
}