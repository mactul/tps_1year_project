#include <stdio.h>
#include "src/io/parser_txt.h"
#include "src/io/parser_bin.h"
#include "src/io/writer_bin.h"
#include "src/arg-handler.h"

int main(int argc, char* argv[])
{
    SA_DynamicArray* films1 = NULL;
    SA_DynamicArray* films2 = NULL;

    Arguments args_structure;
    int index_remaining;

    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        goto EXIT_LBL;
    }

    SA_init();

    films1 = SA_dynarray_create(Film);
    if(films1 == NULL)
    {
        goto EXIT_LBL;
    }

    for (int index = index_remaining; index < argc; index++)
    {
        read_movie_file(films1, argv[index]);
    }

    if (write_films("out/data.bin", films1))
    {
        goto EXIT_LBL;
    }

    films2 = read_all_films("out/data.bin");
    if (films2 == NULL)
    {
        goto EXIT_LBL;
    }
    if (write_films("out/data2.bin", films2))
    {
        goto EXIT_LBL;
    }

EXIT_LBL:
    films_list_free(&films1);
    films_list_free(&films2);

    SA_destroy();
}