#include <stdio.h>
#include <SA/SA.h>
#include "writer_bin.h"

typedef struct _user_data {
    uint32_t rating_count;
    uint32_t sum_note;
} UserData;

static int write_users(FILE* file, SA_DynamicArray* films)
{
    int error_code = 0;

    if (films == NULL)
    {
        return 6;
    }

    SA_DynamicArray* user_datas = SA_dynarray_create_size_hint(UserData, EXPECTED_MAX_USER_ID);

    SA_activate_zero_filling(user_datas);

    uint64_t user_count = 0;
    uint32_t max_user_id = 0;
    for (uint32_t i = 0; i < (uint32_t) SA_dynarray_size(films); i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        for (uint32_t j = 0; j < (uint32_t) SA_dynarray_size(f.ratings); j++)
        {
            Rating rating = SA_dynarray_get(Rating, f.ratings, j);
            UserData data = {.rating_count = 0, .sum_note = 0};
            if (SA_dynarray_size(user_datas) > rating.user_id)
            {
                data = SA_dynarray_get(UserData, user_datas, rating.user_id);
            }
            if (data.rating_count == 0)
            {
                user_count++;
            }
            data.rating_count++;
            data.sum_note += rating.note;
            SA_dynarray_set(UserData, user_datas, rating.user_id, data);
            if(rating.user_id > max_user_id)
            {
                max_user_id = rating.user_id;
            }
        }
    }

    if (fwrite(&user_count, sizeof(user_count), 1, file) <= 0)
    {
        error_code = 1;
        goto QUIT;
    }

    // User id are naturally sorted because they are arranged in the array by indexes.

    for (uint32_t i = 0; i < SA_dynarray_size(user_datas); i++)
    {
        UserData data = SA_dynarray_get(UserData, user_datas, i);
        uint16_t count = (uint16_t)data.rating_count;
        uint16_t avg_note = (uint16_t)((double)data.sum_note * (double)((uint16_t)(-1)) / (double)data.rating_count / 5.0 + 0.5);
        if (data.rating_count == 0)
        {
            continue;
        }
        if (fwrite(&i, sizeof(i), 1, file) <= 0)
        {
            error_code = 2;
            goto QUIT;
        }
        if (fwrite(&count, sizeof(count), 1, file) <= 0)
        {
            error_code = 2;
            goto QUIT;
        }
        if (fwrite(&avg_note, sizeof(avg_note), 1, file) <= 0)
        {
            error_code = 2;
            goto QUIT;
        }
    }

QUIT:
    SA_dynarray_free(&user_datas);
    return error_code;
}

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

    if (fwrite(&len, sizeof(len), 1, file) <= 0)
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
        size_t pos = ftell(file);
        for (uint32_t j = 0; j < f.rating_count; j++)
        {
            Rating r = SA_dynarray_get(Rating, f.ratings, j);
            if (fwrite(&r, sizeof(Rating), 1, file) <= 0)
            {
                error_code = 4;
                goto QUIT;
            }
        }
        size_t end_pos = ftell(file);
        fseek(file, sizeof(uint64_t) + i * sizeof(Film) + offsetof(Film, ratings), SEEK_SET);
        if (fwrite(&pos, sizeof(SA_DynamicArray*), 1, file) <= 0)
        {
            error_code = 5;
            goto QUIT;
        }
        fseek(file, end_pos, SEEK_SET);
    }

    error_code = write_users(file, films);

QUIT:
    if (file != NULL)
    {
        fclose(file);
    }
    return error_code;
}