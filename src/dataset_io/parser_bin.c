#include <stdio.h>
#include "src/dataset_io/parser_bin.h"

typedef struct _bin_reviewer {
    uint32_t user_id;
    uint16_t rate_count;
    uint16_t avg_note;
} BinReviewer;

/// @brief Read all reviewers from binary file
/// @param file Pointer to the binary file
/// @return Array of structures containing every user_id, number of ratings and average rating for this user_id or NULL if there was an error
SA_DynamicArray* read_all_reviewers(FILE* file)
{
    SA_DynamicArray* reviewers = NULL;

    int error_code = 0;
    uint64_t reviewer_count;

    if (file == NULL)
    {
        error_code = 1;
        goto ERROR;
    }

    if (fread(&reviewer_count, sizeof(reviewer_count), 1, file) <= 0)
    {
        error_code = 1;
        goto ERROR;
    }

    reviewers = SA_dynarray_create_size_hint(Reviewer, EXPECTED_MAX_USER_ID);
    SA_activate_zero_filling(reviewers);

    BinReviewer br;
    uint32_t counter = 0;
    for (uint64_t i = 0; i < reviewer_count; i++)
    {
        Reviewer r;
        if (fread(&br, sizeof(BinReviewer), 1, file) <= 0)
        {
            error_code = 1;
            goto ERROR;
        }
        r.avg_note = br.avg_note;
        r.rate_count = br.rate_count;
        SA_dynarray_set(Reviewer, reviewers, br.user_id, r);
        counter++;
    }

ERROR:
    if (!error_code)
    {
        return reviewers;
    }
    SA_dynarray_free(&reviewers);
    return NULL;
}

/// @brief Read all movies from binary file
/// @param file Pointer to the binary file
/// @return Array of structures containing every film_id, number of ratings and all the ratings for this film_id or NULL if there was an error
SA_DynamicArray* read_all_films(FILE* file)
{
    int error_code = 0;

    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }

    uint64_t film_count;
    if (fread(&film_count, sizeof(film_count), 1, file) <= 0)
    {
        error_code = 1;
        goto QUIT;
    }

    SA_DynamicArray* films = SA_dynarray_create_size_hint(Film, film_count + 1); // +1 isn't really useful but just in case

    uint64_t max_seek_offset = 0;

    // Read Film structures
    for (uint64_t i = 0; i < film_count; i++)
    {
        Film f;
        if (fread(&f, sizeof(Film), 1, file) <= 0)
        {
            error_code = 1;
            goto QUIT;
        }
        uint64_t current_position = ftell(file);
        fseek(file, (size_t)f.ratings, SEEK_SET);

        f.ratings = SA_dynarray_create_size_hint(Rating, f.rating_count + 1); // +1 : same as before

        // Read Rating structures
        for (uint32_t j = 0; j < f.rating_count; j++)
        {
            Rating r;
            if (fread(&r, sizeof(Rating), 1, file) <= 0)
            {
                error_code = 3;
                goto QUIT;
            }
            SA_dynarray_append(Rating, f.ratings, r);
        }

        uint64_t rate_seek_end_offset = ftell(file);
        if (rate_seek_end_offset > max_seek_offset)
        {
            max_seek_offset = rate_seek_end_offset;
        }

        SA_dynarray_append(Film, films, f);
        fseek(file, current_position, SEEK_SET);
    }

    fseek(file, max_seek_offset, SEEK_SET);

QUIT:
    // Free memory if needed
    if (file == NULL)
    {
        return NULL;
    }
    if (!error_code)
    {
        return films;
    }
    printf("%d\n", error_code);
    for (uint64_t i = 0; i < SA_dynarray_size(films); i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        SA_dynarray_free(&(f.ratings));
    }
    SA_dynarray_free(&films);
    return NULL;
}