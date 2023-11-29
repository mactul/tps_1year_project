#include <stdio.h>
#include <SA/SA.h>
#include "writer_bin.h"


int write_films(const char* out_filename, SA_DynamicArray* films)
{
    long last_slash = -1;
    char* out_folder = NULL;

    if (films == NULL)
    {
        return 6;
    }
    
    out_folder = (char*) SA_malloc((SA_strlen(out_filename) + 1) * sizeof(char));
    SA_strcpy(out_folder, out_filename);

    for (int i = 0; i < SA_strlen(out_filename); i++)
    {
        if (out_folder[i] == '/')
        {
            last_slash = i;
        }
    }
    if (last_slash != -1)
    {
        out_folder[last_slash] = '\0';
        SA_recursive_mkdir(out_folder);
    }

    SA_free(&out_folder);

    int error_code = 0;
    FILE* file = fopen(out_filename, "wb");
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
                error_code = 4;
                goto QUIT;
            }
        }
        uint64_t end_pos = ftell(file);
        fseek(file, sizeof(uint64_t) + i * sizeof(Film) + 16, SEEK_SET);
        if (fwrite(&pos, sizeof(long), 1, file) <= 0)
        {
            error_code = 5;
            goto QUIT;
        }
        fseek(file, end_pos, SEEK_SET);
    }

QUIT:
    if (file != NULL)
    {
        fclose(file);
    }
    return error_code;
}