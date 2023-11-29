#include "writer_bin.h"

void make_path(char* out_folder)
{
    char* out_copy = malloc((strlen(out_folder) + 1) * sizeof(char));
    strcpy(out_copy, out_folder);

    long last_slash = -1;

    for (size_t i = 0; i < strlen(out_folder); i++)
    {
        if (out_folder[i] == '/')
        {
            last_slash = i;
        }
    }

    if (last_slash == -1)
    {
        struct stat st = {0};
        if (stat(out_copy, &st) == -1)
        {
            mkdir(out_copy, 0755);
        }
        free(out_copy);
        return;
    }
    
    out_folder[last_slash] = '\0';

    make_path(out_folder);

    struct stat st = {0};
    if (stat(out_copy, &st) == -1)
    {
        mkdir(out_copy, 0755);
    }

    free(out_copy);
}

int write_films(const char* out_filename, SA_DynamicArray* films)
{
    char* out_folder = malloc((strlen(out_filename) + 1) * sizeof(char));
    strcpy(out_folder, out_filename);

    long last_slash = -1;

    for (size_t i = 0; i < strlen(out_filename); i++)
    {
        if (out_folder[i] == '/')
        {
            last_slash = i;
        }
    }

    if (last_slash != -1)
    {
        out_folder[last_slash] = '\0';
        make_path(out_folder);
    }

    free(out_folder);

    if (films == NULL)
    {
        return 6;
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
                error_code = 4;
                goto QUIT;
            }
        }
        fseek(file, sizeof(uint64_t) + i * sizeof(Film) + 16, SEEK_SET);
        if (fwrite(&pos, sizeof(long), 1, file) <= 0)
        {
            error_code = 5;
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