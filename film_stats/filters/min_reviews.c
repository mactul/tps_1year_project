#include "film_stats/filters/min_reviews.h"
#include <SA/memory/mem_alloc.h>

uint32_t review_count_for_user(const SA_DynamicArray* reviewers, uint32_t user_id)
{
    uint32_t min_index = 0;
    uint32_t max_index = SA_dynarray_size(reviewers)-1;
    
    do
    {
        uint32_t current_index = (min_index + max_index) / 2;
        Reviewer r = SA_dynarray_get(Reviewer, reviewers, current_index);
        if(r.user_id > user_id)
        {
            max_index = current_index;
        }
        else if(r.user_id < user_id)
        {
            min_index = current_index;
        }
        else
        {
            return r.rate_count;
        }
    } while(min_index < max_index-1);

    return 0;
}

void filter_min_reviews(Film* film_filtered, const Film* film_to_filter, const SA_DynamicArray* reviewers, uint32_t min)
{
    film_filtered->film_id = film_to_filter->film_id;
    film_filtered->ratings = SA_dynarray_create_size_hint(Rating, EXPECTED_RATINGS_PER_FILM_NUMBER);
    film_filtered->rating_count = 0;

    for (uint64_t i = 0; i < SA_dynarray_size(film_to_filter->ratings); i++)
    {
        Rating r = SA_dynarray_get(Rating, film_to_filter->ratings, i);
        if (review_count_for_user(reviewers, r.user_id) >= min)
        {
            film_filtered->rating_count++;
            SA_dynarray_append(Rating, film_filtered->ratings, r);
        }
    }
}