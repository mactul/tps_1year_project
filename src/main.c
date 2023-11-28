#include <stdio.h>
#include "src/io/parser_txt.h"
#include "src/io/parser_bin.h"
#include "src/io/writer_bin.h"

int main()
{
    SA_init();

    SA_DynamicArray* films = SA_dynarray_create(Film);
    read_movie_file(films, "download/training_set/mv_0000001.txt");
    if (write_films("out/data.bin", films))
    {
        return 2;
    }
    uint64_t length = SA_dynarray_size(films);
    for (uint64_t i = 0; i < length; i++)
    {
        Film f = SA_dynarray_get(Film, films, i);
        SA_dynarray_free(&f.ratings);
    }
    SA_dynarray_free(&films);

    SA_DynamicArray* films2 = read_all_films("out/data.bin");
    if (films2 == NULL)
    {
        return 2;
    }
    if (write_films("out/data2.bin", films2))
    {
        return 2;
    }
    uint64_t length2 = SA_dynarray_size(films2);
    for (uint64_t i = 0; i < length2; i++)
    {
        Film f = SA_dynarray_get(Film, films2, i);
        SA_dynarray_free(&f.ratings);
    }
    SA_dynarray_free(&films2);

    SA_destroy();
}