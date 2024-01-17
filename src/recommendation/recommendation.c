#include <SA/SA.h>
#include <math.h>
#include <stdlib.h>
#include "src/data_structs.h"

#define FACTOR 10000.0

#define NOTE_IMPACT 0.05

/// @brief Compare doubles (used by qsort)
/// @param e1 Pointer to the first double
/// @param e2 Pointer to the second double
/// @return > 0 if the first double is higher than the second, < 0 if the second one is higher, 0 if they are equal
static inline int cmp_doubles(const void* e1, const void* e2)
{
    return *((double*)e1) - *((double*)e2);
}

/// @brief Normalize a rating from a user
/// @param note A rating from a user
/// @param avg_note The average rating for this user
/// @return The rating normalized according to the averating for a user
static double normalize_note(double note, double avg_note)
{
    avg_note = avg_note / (double)((uint16_t)-1);
    
    return ((note / 5.0) / avg_note) / 5.0;
}

/// @brief Get the distance between two movies according to their ratings
/// @param film1 Pointer to the first movie
/// @param film2 Pointer to the second movie
/// @param reviewers Array of structures containing the number of ratings and average rating for every user
/// @return A value in the range [0-1], 0 means the films are equivalent, near 1 means they are complete opposites, 1 means at least one of the movies has no review 
double distance_between_films(const Film* film1, const Film* film2, const SA_DynamicArray* reviewers)
{
    double ratings_taken = 0.0;
    double distance_sum = 0.0;
    uint64_t i1 = 0;
    uint64_t i2 = 0;

    while(i1 < SA_dynarray_size(film1->ratings) && i2 < SA_dynarray_size(film2->ratings))
    {
        Rating* r1 = _SA_dynarray_get_element_ptr(film1->ratings, i1);
        Rating* r2 = _SA_dynarray_get_element_ptr(film2->ratings, i2);
        if(r1->user_id < r2->user_id)
        {
            distance_sum += 1.0 / FACTOR;
            ratings_taken += 1.0 / FACTOR;
            i1++;
        }
        else if(r2->user_id < r1->user_id)
        {
            distance_sum += 1.0 / FACTOR;
            ratings_taken += 1.0 / FACTOR;
            i2++;
        }
        else
        {
            Reviewer user = SA_dynarray_get(Reviewer, reviewers, r1->user_id);
            distance_sum += fabs(normalize_note(r1->note, user.avg_note) - normalize_note(r2->note, user.avg_note));
            ratings_taken += 1.0;
            i1++;
            i2++;
        }
    }
    if (ratings_taken == 0.0)
    {
        return 1.0;
    }
    return distance_sum / (ratings_taken);
}

/// @brief Get the score of a specific film according to all the liked films and its ratings
/// @param film Pointer to the film
/// @param liked_films Array of liked films
/// @param avg_film_note Average rating of this movie
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @return The score [0-1] of this film, 1 means it has the highest recommendation, 0 means it has the lowest
double calculate_recommendation(const Film* film, const SA_DynamicArray* liked_films, double avg_film_note, const SA_DynamicArray* reviewers)
{
    if(liked_films == NULL || SA_dynarray_size(liked_films) == 0)
    {
        return avg_film_note / 5.0;
    }
    int n = SA_dynarray_size(liked_films);
    double* distances = SA_malloc(n * sizeof(double));
    double coef = 1.0;
    double total_coefs = 0.0;
    double total = 0.0;

    for(int i = 0; i < n; i++)
    {
        distances[i] = distance_between_films(film, _SA_dynarray_get_element_ptr(liked_films, i), reviewers);
    }

    qsort(distances, n, sizeof(double), cmp_doubles);

    for(int i = 0; i < n; i++)
    {
        total += coef * (1.0 - distances[i]);
        total_coefs += coef;
        coef /= 2;
    }
    total = total / total_coefs;
    
    SA_free(&distances);

    return (total * (double)n + NOTE_IMPACT * avg_film_note / 5.0) / ((double)n + NOTE_IMPACT);
}