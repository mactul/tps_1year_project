#include "src/data_structs.h"

/// @brief Free an array to a Film structure (as this structure also contains a pointer)
/// @param films Pointer to an array of Film structure
void films_list_free(SA_DynamicArray** films)
{
    if(*films == NULL)
    {
        return;
    }
    uint64_t length = SA_dynarray_size(*films);
    for (uint64_t i = 0; i < length; i++)
    {
        Film f = SA_dynarray_get(Film, *films, i);
        SA_dynarray_free(&(f.ratings));
    }
    SA_dynarray_free(films);
}