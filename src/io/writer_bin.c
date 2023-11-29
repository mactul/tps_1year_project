#include "writer_bin.h"

int write_films(const char* out_filename, SA_DynamicArray* films)
{
    if (films == NULL)
    {
        return 2;
    }

    int error_code = 0;
    FILE* file = fopen(out_filename, "w+");
    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }
    uint64_t len = SA_dynarray_size(films);

    if (fwrite(&len, sizeof(uint64_t), 1, file) <= 0)
    {
        error_code = 2;
        goto QUIT;
    }

    // Write Film structures
    for (uint64_t i = 0; i < len; i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        if (fwrite(&f, sizeof(Film), 1, file) <= 0)
        {
            error_code = 3;
            goto QUIT;
        }
    }

    // Write Rating structures
    for (uint64_t i = 0; i < len; i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        uint64_t pos = ftell(file);
        for (uint64_t j = 0; j < f.rating_count; j++)
        {
            Rating r = SA_dynarray_get(Rating, f.ratings, j);
            if (fwrite(&r, sizeof(Rating), 1, file) <= 0)
            {
                error_code = 2;
                goto QUIT;
            }
        }
        fseek(file, sizeof(uint64_t) + i * sizeof(Film) + 16, SEEK_SET);
        if (fwrite(&pos, sizeof(long), 1, file) <= 0)
        {
            error_code = 4;
            goto QUIT;
        }
    }

QUIT:
    if (file != NULL)
    {
        fclose(file);
    }
    return error_code;
}