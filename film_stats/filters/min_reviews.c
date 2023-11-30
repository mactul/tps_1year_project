#include "film_stats/filters/min_reviews.h"
#include <SA/memory/mem_alloc.h>

uint32_t review_count_for_user(SA_DynamicArray* reviewers, uint32_t user_id)
{
    for (uint32_t i = 0; i < (uint32_t) SA_dynarray_size(reviewers); i++)
    {
        Reviewer r = SA_dynarray_get(Reviewer, reviewers, i);
        if (r.user_id == user_id)
        {
            return r.rate_count;
        }
    }
    return 0;
}

Film* filter_min_reviews(const Film* film, SA_DynamicArray* reviewers, uint32_t min)
{
    Film* without_reviews = SA_malloc(sizeof(Film));
    if (without_reviews == NULL)
    {
        return NULL;
    }
    without_reviews->film_id = film->film_id;
    without_reviews->ratings = SA_dynarray_create(Rating);
    without_reviews->rating_count = 0;

    for (uint64_t i = 0; i < SA_dynarray_size(film->ratings); i++)
    {
        Rating r = SA_dynarray_get(Rating, film->ratings, i);
        if (review_count_for_user(reviewers, r.user_id) <= min)
        {
            continue;
        }
        without_reviews->rating_count++;
        SA_dynarray_append(Rating, without_reviews->ratings, r);
    }

    return without_reviews;
}