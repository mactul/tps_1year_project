#include <stdio.h>
#include <inttypes.h>
#include "film_stats/filters/min_reviews.h"
#include "film_stats/filters/limit_date.h"
#include "film_stats/filters/reviewers_list.h"
#include "film_stats/calculate_stats.h"
#include "src/recommendation/recommendation.h"
#include "src/recommendation/parse_liked_films.h"

/// @brief Compare recommendations between two movies (used by qsort)
/// @param e1 A movie stats structure pointer
/// @param e2 Another movie stat structure pointer
/// @return -1 if the first movie has a higher recommendation, 1 if the second one has a higher recommendation, 0 if the recommendations are equal
static inline int cmp_recommendations(const void* e1, const void* e2)
{
    if(((FilmStats*)e1)->recommendation < ((FilmStats*)e2)->recommendation)
    {
        return 1;
    }
    else if(((FilmStats*)e1)->recommendation > ((FilmStats*)e2)->recommendation)
    {
        return -1;
    }
    return 0;
}

/// @brief Returns the year of the most recent review
/// @param ratings An array of ratings structure containing the author, rating and date
/// @return The most recent year if it exists, otherwise YEARS_OFFSET
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

/// @brief Add film stats of a movie
/// @param film_stats Array of stats for each movie
/// @param film_filtered Pointer to a film with filtered reviews
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param liked_films Array of liked films
static void add_film_stats(SA_DynamicArray* film_stats, const Film* film_filtered, const SA_DynamicArray* reviewers, const SA_DynamicArray* liked_films)
{
    uint64_t note_sum = 0;
    uint64_t nb_note = 0;
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

        note_sum += rating.note;
        nb_note++;
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

    stats.recommendation = calculate_recommendation(film_filtered, liked_films, (double)note_sum / (double)nb_note, reviewers);

    SA_dynarray_append(FilmStats, film_stats, stats);
}

/// @brief Applies all filters to a movie structure pointer
/// @param film_filtered The movie with all enabled filters applied
/// @param film_to_filter The movie on which to apply the filters
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param filter_options The filters to apply
/// @return SA_TRUE if at least one filter was applied, else SA_FALSE
static SA_bool apply_all_filters(Film* film_filtered, const Film* film_to_filter, const SA_DynamicArray* reviewers, const StatsArguments* filter_options)
{
    Film film_to_filter_copy = *film_to_filter;
    SA_bool filter_applied = SA_FALSE;

    if(filter_options->min_reviews != -1)
    {
        filter_min_reviews(film_filtered, &film_to_filter_copy, reviewers, filter_options->min_reviews);
        if(filter_applied)
        {
            SA_dynarray_free(&(film_to_filter_copy.ratings));
        }
        film_to_filter_copy = *film_filtered;
        filter_applied = SA_TRUE;
    }

    if(filter_options->limit != NULL)
    {
        uint16_t year;
        uint8_t month, day;

        sscanf(filter_options->limit, "%" SCNu16 "-%" SCNu8 "-%" SCNu8, &year, &month, &day);
        filter_date_from(film_filtered, &film_to_filter_copy, year - YEARS_OFFSET, month, day, SA_FALSE);
        if(filter_applied)
        {
            SA_dynarray_free(&(film_to_filter_copy.ratings));
        }
        film_to_filter_copy = *film_filtered;
        filter_applied = SA_TRUE;
    }

    if (filter_options->bad_reviewers != NULL)
    {
        filter_reviewers(film_filtered, &film_to_filter_copy, filter_options->bad_reviewers, SA_FALSE);
        if(filter_applied)
        {
            SA_dynarray_free(&(film_to_filter_copy.ratings));
        }
        film_to_filter_copy = *film_filtered;
        filter_applied = SA_TRUE;
    }

    if (filter_options->only_reviewers != NULL)
    {
        filter_reviewers(film_filtered, &film_to_filter_copy, filter_options->only_reviewers, SA_TRUE);
        if(filter_applied)
        {
            SA_dynarray_free(&(film_to_filter_copy.ratings));
        }
        film_to_filter_copy = *film_filtered;
        filter_applied = SA_TRUE;
    }
    
    return filter_applied;
}

/// @brief Calculate stats for every movie in the dataset
/// @param films Array of movies
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param filter_options Pointer to a structure containing all the filters to apply
/// @return Film stats for every movie
SA_DynamicArray* calculate_all_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const StatsArguments* filter_options)
{
    SA_DynamicArray* film_stats = SA_dynarray_create_size_hint(FilmStats, EXPECTED_FILM_NUMBERS);
    SA_DynamicArray* liked_films = NULL;
    if (filter_options->liked_films_filepath != NULL)
    {
        liked_films = parse_liked_films(filter_options->liked_films_filepath, films);
    }
    for(uint64_t i = 0; i < SA_dynarray_size(films); i++)
    {
        Film film_filtered;
        if(apply_all_filters(&film_filtered, _SA_dynarray_get_element_ptr(films, i), reviewers, filter_options))
        {
            add_film_stats(film_stats, &film_filtered, reviewers, liked_films);
            SA_dynarray_free(&(film_filtered.ratings));
        }
        else
        {
            add_film_stats(film_stats, _SA_dynarray_get_element_ptr(films, i), reviewers, liked_films);
        }
        if(i % (SA_dynarray_size(films) / 100) == 0)
        {
            printf("%d%%\n\033[A\r", 100 * (int)i / (int)SA_dynarray_size(films));
        }
    }
    SA_dynarray_free(&liked_films);

    SA_dynarray_qsort(film_stats, cmp_recommendations);

    return film_stats;
}