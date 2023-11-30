#include "src/data_structs.h"

static uint32_t get_max_year(SA_DynamicArray* ratings)
{
    uint8_t max_year_offset = 0;
    Rating* rating_array = (Rating*) _SA_dynarray_get_element_ptr(ratings, 0);
    for(uint64_t i = 0; i < SA_dynarray_size(ratings); i++)
    {
        if(rating_array[i].offseted_year > max_year_offset)
        {
            max_year_offset = rating_array[i].offseted_year;
        }
    }
    return (uint32_t)max_year_offset + YEARS_OFFSET;
}

void calculate_stats(SA_DynamicArray* film_stats, const Film* film_filtered)
{
    uint64_t sum_ratings[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    FilmStats stats = {0};

    stats.film_id = film_filtered->film_id;
    stats.max_year = get_max_year(film_filtered->ratings);
    for(uint64_t i = 0; i < SA_dynarray_size(film_filtered->ratings); i++)
    {
        Rating rating = SA_dynarray_get(Rating, film_filtered->ratings, i);
        uint32_t year_index = stats.max_year - (YEARS_OFFSET + (uint32_t)rating.offseted_year);
        if(year_index >= NUMBER_OF_YEARS_LOGGED_IN_STATS)
        {
            continue;
        }
        sum_ratings[year_index] += rating.note;
        stats.kept_rating_count_over_years[year_index]++;
    }
    for(int i = 0; i < NUMBER_OF_YEARS_LOGGED_IN_STATS; i++)
    {
        stats.mean_rating_over_years[i] = (float)sum_ratings[i] / (float)stats.kept_rating_count_over_years[i];
    }

    SA_dynarray_append(FilmStats, film_stats, stats);
}