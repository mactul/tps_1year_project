#include "film_stats/filters/reviewers_list.h"
#include <string.h>
#include <SA/strings/strings.h>
#include <SA/memory/mem_alloc.h>
#include <stdio.h>

/// @brief Check if a user_id belongs to an array of user_ids
/// @param user_ids The array to check
/// @param user_id The value to find
/// @return SA_TRUE if user_id is in user_ids, else SA_FALSE
static SA_bool is_user_id_in_array(SA_DynamicArray* user_ids, uint32_t user_id)
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

/// @brief Filter all ratings from user_ids
/// @param film_filtered Pointer to the film with filtered reviews
/// @param film_to_filter Pointer to the film with all the reviews
/// @param user_ids Array of user_ids
/// @param whitelist_mode SA_TRUE if the filter is a whitelist, SA_FALSE if the filter is a blacklists
static void filter_user_ids(Film* film_filtered, const Film* film_to_filter, SA_DynamicArray* user_ids, SA_bool whitelist_mode)
{
    uint32_t rating_count = film_to_filter->rating_count;
    for (uint32_t i = 0; i < rating_count; i++)
    {
        Rating rating = SA_dynarray_get(Rating, film_to_filter->ratings, i);
        SA_bool isinreviews = is_user_id_in_array(user_ids, rating.user_id);
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

/// @brief Parse all reviewer ids from a comma-separated string to call filter_user_ids
/// @param film_filtered Pointer to the film with the reviewers filtered
/// @param film_to_filter Pointer to the film with all the reviews
/// @param reviewers Comma-separated list of reviewer ids
/// @param whitelist_mode SA_TRUE if the filter is a whitelist, SA_FALSE if the filter is a blacklists
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