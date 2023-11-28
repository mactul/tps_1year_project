#include "movie-rating.h"

void append_rating(uint64_t movie_id, uint8_t rating, Movie* movies)
{
    for (int i = 0; i < 17770; i++)
    {
        if (movies[i].film_id != movie_id)
        {
            continue;
        }
        movies[i].average_rating += rating;
        movies[i].rating_count += 1;
    }
}