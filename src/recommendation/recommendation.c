#include <SA/SA.h>
#include <math.h>
#include "src/data_structs.h"

#define FACTOR 10000.0

static double normalize_note(double note, double avg_note)
{
    avg_note = avg_note / (double)((uint16_t)-1);
    
    return ((note / 5.0) / avg_note) / 5.0;
}

double distance_between_films(Film* film1, Film* film2, SA_DynamicArray* reviewers)
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
    return distance_sum / (ratings_taken);
}