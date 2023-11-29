#include "parser_bin.h"

SA_DynamicArray* read_all_films(const char* in_filename)
{
    int error_code = 0;

    FILE* file = fopen(in_filename, "r");
    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }

    SA_DynamicArray* films = SA_dynarray_create_size_hint(Film, EXPECTED_FILM_NUMBERS);
    uint64_t film_count;
    fread(&film_count, sizeof(uint64_t), 1, file);

    // Read Film structures
    for (uint64_t i = 0; i < film_count; i++)
    {
        Film f;
        if (fread(&f, sizeof(Film) - sizeof(uint64_t), 1, file) <= 0)
        {
            error_code = 1;
            goto QUIT;
        }
        uint64_t offset = 0;
        if (fread(&offset, sizeof(uint64_t), 1, file) <= 0)
        {
            error_code = 1;
            goto QUIT;
        }
        uint64_t current_position = ftell(file);
        fseek(file, offset, SEEK_SET);
        f.ratings = SA_dynarray_create_size_hint(Rating, EXPECTED_RATINGS_PER_FILM_NUMBER);

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
        SA_dynarray_append(Film, films, f);
        fseek(file, current_position, SEEK_SET);
    }

QUIT:
    // Free memory if needed
    if (file == NULL)
    {
        return NULL;
    }
    fclose(file);
    if (!error_code)
    {
        return films;
    }
    printf("%d\n", error_code);
    for (uint64_t i = 0; i < film_count; i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        SA_dynarray_free(&f.ratings);
        SA_dynarray_free(&films);
    }
    return NULL;
}