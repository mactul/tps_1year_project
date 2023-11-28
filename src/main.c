#include <stdio.h>
#include "src/io/parser_txt.h"

int main()
{
    SA_DynamicArray* films = SA_dynarray_create(Film);
    read_movie_file(films, "download/training_set/mv_0000001.txt");
}