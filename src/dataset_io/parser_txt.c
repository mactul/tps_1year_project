#include <stdio.h>
#include <inttypes.h>
#include <SA/memory/dynamic_array.h>
#include <SA/strings/strings.h>
#include "src/dataset_io/parser_txt.h"

#define BUFFER_SIZE 256

/// @brief Compare ratings by user_id (used by qsort)
/// @param el1 Pointer to the first rating structure
/// @param el2 Pointer to the second rating structure
/// @return > 0 when the first user_id is higher than the second, < 0 when the second user_id is higher, 0 if they are equal
static int compare_ratings(const void* el1, const void* el2)
{
    return ((Rating*)el1)->user_id - ((Rating*)el2)->user_id;
}

/// @brief Read a number from a string and move its pointer after the number
/// @param line_to_destroy Pointer to a string containing numbers
/// @return The longest number found
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

/// @brief Parse the rating line from a user_id,rating,YYYY-MM-DD formatted string
/// @param line_to_destroy The string containg all this information
/// @param rating The rating structure to fill with this information
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

/// @brief Parse the movie line from a film_id,YYYY,name formatted string
/// @param line_to_destroy The string containing all this information
/// @param film_info The film structure to fill with this information
/// @return the film_id
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

/// @brief Read reviews about a movie from its .txt text file and add it to an array
/// @param films Array of movies
/// @param filename File path of the .txt text file
/// @return 0 if everything went correctly, 1 if the file doesn't exist
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
    if (file != NULL)
    {
        fclose(file);
    }
    return error_code;
}

/// @brief Read all the movies release year and name from the movie_titles.txt text file
/// @param movie_titles_filepath File path of the movie_titles.txt text file
/// @return Array of structures containing the release year and name for every movie
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