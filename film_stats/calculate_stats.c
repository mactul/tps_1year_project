#include <stdio.h>
#include <inttypes.h>
#include "film_stats/filters/min_reviews.h"
#include "film_stats/filters/limit_date.h"
#include "film_stats/calculate_stats.h"


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

static void add_film_stats(SA_DynamicArray* film_stats, const Film* film_filtered)
{
    uint64_t sum_ratings[NUMBER_OF_YEARS_LOGGED_IN_STATS] = {0};
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
        if(stats.kept_rating_count_over_years[i] == 0)
        {
            stats.mean_rating_over_years[i] = -1.0;
        }
        else
        {
            stats.mean_rating_over_years[i] = (float)sum_ratings[i] / (float)stats.kept_rating_count_over_years[i];
        }
    }

    SA_dynarray_append(FilmStats, film_stats, stats);
}

static SA_bool apply_all_filters(Film* film_filtered, const Film* film_to_filter, const SA_DynamicArray* reviewers, const Arguments* filter_options)
{
    if(filter_options->min_reviews != -1)
    {
        filter_min_reviews(film_filtered, film_to_filter, reviewers, filter_options->min_reviews);
        return SA_TRUE;
    }
    if(filter_options->limit != NULL)
    {
        uint16_t year;
        uint8_t month, day;

        sscanf(filter_options->limit, "%" SCNu16 "-%" SCNu8 "-%" SCNu8, &year, &month, &day);
        filter_date_from(film_filtered, film_to_filter, year - YEARS_OFFSET, month, day, SA_FALSE);
        return SA_TRUE;
    }
    return SA_FALSE;
}

SA_DynamicArray* calculate_all_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const Arguments* filter_options)
{
    SA_DynamicArray* film_stats = SA_dynarray_create_size_hint(FilmStats, EXPECTED_FILM_NUMBERS);
    for(uint64_t i = 0; i < SA_dynarray_size(films); i++)
    {
        Film film_filtered;
        if(apply_all_filters(&film_filtered, _SA_dynarray_get_element_ptr(films, i), reviewers, filter_options))
        {
            add_film_stats(film_stats, &film_filtered);
            SA_dynarray_free(&(film_filtered.ratings));
        }
        else
        {
            add_film_stats(film_stats, _SA_dynarray_get_element_ptr(films, i));
        }
    }

    return film_stats;
}