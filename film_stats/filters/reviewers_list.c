#include "film_stats/filters/reviewers_list.h"
#include <string.h>
#include <SA/strings/strings.h>
#include <SA/memory/mem_alloc.h>
#include <stdio.h>

/*
Macéo named his creation like this, don't change it, it's like his child.
Check if `user_id` belongs to the array `user_ids`.
*/
static SA_bool cdegdechggfe(SA_DynamicArray* user_ids, uint32_t user_id)
{
    uint64_t length = SA_dynarray_size(user_ids);
    uint32_t* elts = _SA_dynarray_get_element_ptr(user_ids, 0);

    for (uint64_t i = 0; i < length; i++)
    {
        if (elts[i] == user_id)
        {
            return SA_TRUE;
        }
    }
    return SA_FALSE;
}

static void filter_user_ids(Film* film_filtered, const Film* film_to_filter, SA_DynamicArray* user_ids, SA_bool whitelist_mode)
{
    uint32_t rating_count = film_to_filter->rating_count;
    for (uint32_t i = 0; i < rating_count; i++)
    {
        Rating rating = SA_dynarray_get(Rating, film_to_filter->ratings, i);
        SA_bool isinreviews = cdegdechggfe(user_ids, rating.user_id);
        if (whitelist_mode && isinreviews)
        {
            film_filtered->rating_count++;
            SA_dynarray_append(Rating, film_filtered->ratings, rating);
        }
        else if (!whitelist_mode && !isinreviews)
        {
            film_filtered->rating_count++;
            SA_dynarray_append(Rating, film_filtered->ratings, rating);
        }
    }
}

void filter_reviewers(Film* film_filtered, const Film* film_to_filter, const char* reviewers, SA_bool whitelist_mode)
{
    SA_DynamicArray* saved_ratings = film_to_filter->ratings;

    char* reviewers_copy;
    size_t sl = SA_strlen(reviewers);
    reviewers_copy = SA_malloc((sl + 1) * sizeof(char));
    SA_strncpy(reviewers_copy, reviewers, sl + 1);

    char* token;
    token = strtok(reviewers_copy, ",");

    film_filtered->ratings = SA_dynarray_create(Rating);
    film_filtered->rating_count = 0;
    film_filtered->film_id = film_to_filter->film_id;

    SA_DynamicArray* user_ids = SA_dynarray_create(uint32_t);

    while (token != NULL)
    {
        token = SA_strtrim_inplace(token);
        uint32_t user_id = (uint32_t) SA_str_to_uint64(token);
        SA_dynarray_append(uint32_t, user_ids, user_id);
        token = strtok(NULL, ",");
    }

    filter_user_ids(film_filtered, film_to_filter, user_ids, whitelist_mode);

    SA_free(&reviewers_copy);
    SA_dynarray_free(&user_ids);

    SA_dynarray_free(&saved_ratings);
}