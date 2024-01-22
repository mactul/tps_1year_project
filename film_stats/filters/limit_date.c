#include "film_stats/filters/limit_date.h"
#include <SA/memory/mem_alloc.h>

/// @brief Compare a rating's date with another date
/// @param r The rating to compare
/// @param offsetted_year A year (minus OFFSETTED_YEAR)
/// @param month A month (0-11)
/// @param day A day (0-31)
/// @return -1 if the rating's date is older, 1 if it is newer, 0 if the dates are equal
static int8_t compare_rating_date(Rating r, uint8_t offsetted_year, uint8_t month, uint8_t day)
{
    if (r.offseted_year < offsetted_year)
    {
        return -1;
    }
    else if (r.offseted_year > offsetted_year)
    {
        return 1;
    }
    if (r.month < month)
    {
        return -1;
    }
    else if (r.month > month)
    {
        return 1;
    }
    if (r.day < day)
    {
        return -1;
    }
    else if (r.day > day)
    {
        return 1;
    }
    return 0;
}

/// @brief Filter all reviews from a certain date
/// @param film_filtered Pointer to the film with filtered reviews
/// @param film_to_filter Pointer to the film with all the reviews
/// @param offsetted_year Year (minus OFFSETTED_YEAR) limit
/// @param month Month limit
/// @param day Day limit
/// @param date_after SA_TRUE if we want only reviews after the date, SA_FALSE if we want reviews before
void filter_date_from(Film* film_filtered, const Film* film_to_filter, uint8_t offsetted_year, uint8_t month, uint8_t day, SA_bool date_after)
{
    film_filtered->film_id = film_to_filter->film_id;
    film_filtered->ratings = SA_dynarray_create(Rating);
    film_filtered->rating_count = 0;

    for (uint64_t i = 0; i < SA_dynarray_size(film_to_filter->ratings); i++)
    {
        Rating r = SA_dynarray_get(Rating, film_to_filter->ratings, i);
        if ((date_after * 2 - 1) * compare_rating_date(r, offsetted_year, month, day) >= 0) // if date_after == true, take the positive value of comparison, else take negative value
        {
            SA_dynarray_append(Rating, film_filtered->ratings, r);
            film_filtered->rating_count++;
        }
    }
}
