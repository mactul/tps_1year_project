#include <stdio.h>
#include "SA/strings/strings.h"
#include "src/data_structs.h"

#define BUFFER_SIZE 256

/// @brief Find the first occurence of a number in a string
/// @param line_to_destroy The string in which we want to find a number
/// @return Pointer to the string right before this number with a NULL character right after the number
static char* get_number_in_line(char* line_to_destroy)
{
    int i = 0;
    while(!SA_CHAR_IS_DIGIT(*line_to_destroy))
    {
        line_to_destroy++;
    }
    while(SA_CHAR_IS_DIGIT(line_to_destroy[i]))
    {
        i++;
    }
    line_to_destroy[i] = '\0';

    return line_to_destroy;
}

/// @brief Find a film by film_id (dichotomy)
/// @param films Array of film structures
/// @param film_id The id to find
/// @return Pointer to this film if it exists, else NULL
static Film* get_film(const SA_DynamicArray* films, uint32_t film_id)
{
    uint32_t min_index = 0;
    uint32_t max_index = SA_dynarray_size(films)-1;
    
    do
    {
        uint32_t current_index = (min_index + max_index) / 2;
        Film* film = _SA_dynarray_get_element_ptr(films, current_index);
        if(film->film_id > film_id)
        {
            max_index = current_index;
        }
        else if(film->film_id < film_id)
        {
            min_index = current_index;
        }
        else
        {
            return film;
        }
    } while(min_index < max_index-1);

    return NULL;
}

/// @brief Parse liked films from a text file
/// @param liked_films_filepath File path to this text file
/// @param films Array of existing films
/// @return Array of existing liked films if the file exists, else NULL
SA_DynamicArray* parse_liked_films(const char* liked_films_filepath, const SA_DynamicArray* films)
{
    char buffer[BUFFER_SIZE];
    FILE* file = fopen(liked_films_filepath, "r");
    SA_DynamicArray* liked_films = NULL;
    if(file == NULL)
    {
        return NULL;
    }

    liked_films = SA_dynarray_create(Film);

    while(fgets(buffer, BUFFER_SIZE, file) != NULL)
    {
        uint32_t film_id = SA_str_to_uint64(get_number_in_line(buffer));
        Film* film = get_film(films, film_id);
        if(film != NULL)
        {
            SA_dynarray_append(Film, liked_films, *film);
        }
    }

    fclose(file);

    return liked_films;
}