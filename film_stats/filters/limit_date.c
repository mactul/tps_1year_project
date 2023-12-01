#include "film_stats/filters/limit_date.h"
#include <SA/memory/mem_alloc.h>

int8_t compare_rating_date(Rating r, uint8_t offsetted_year, uint8_t month, uint8_t day)
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
            SA_dynarray_append(Rating, film_to_filter->ratings, r);
            film_filtered->rating_count++;
        }
    }
}
