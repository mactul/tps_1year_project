#include "film_stats/filters/min_reviews.h"
#include <SA/memory/mem_alloc.h>

/// @brief Filter all reviews where the reviewer has less than min reviews
/// @param film_filtered Pointer to the film with filtered reviews
/// @param film_to_filter Pointer to the film with all the reviews
/// @param reviewers Array of structures containing the number of ratings and average rate for every reviewer
/// @param min The minimum number of reviews
void filter_min_reviews(Film* film_filtered, const Film* film_to_filter, const SA_DynamicArray* reviewers, uint32_t min)
{
    film_filtered->film_id = film_to_filter->film_id;
    film_filtered->ratings = SA_dynarray_create_size_hint(Rating, EXPECTED_RATINGS_PER_FILM_NUMBER);
    film_filtered->rating_count = 0;

    for (uint64_t i = 0; i < SA_dynarray_size(film_to_filter->ratings); i++)
    {
        Rating r = SA_dynarray_get(Rating, film_to_filter->ratings, i);
        Reviewer user = SA_dynarray_get(Reviewer, reviewers, r.user_id);
        if (user.rate_count >= min)
        {
            film_filtered->rating_count++;
            SA_dynarray_append(Rating, film_filtered->ratings, r);
        }
    }
}