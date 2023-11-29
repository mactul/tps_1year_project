#include "src/data_structs.h"

void films_list_free(SA_DynamicArray** films)
{
    uint64_t length = SA_dynarray_size(*films);
    for (uint64_t i = 0; i < length; i++)
    {
        Film f = SA_dynarray_get(Film, *films, i);
        SA_dynarray_free(&(f.ratings));
    }
    SA_dynarray_free(films);
}