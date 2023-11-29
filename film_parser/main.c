#include <stdio.h>
#include <SA/SA.h>
#include "src/io/parser_txt.h"
#include "src/io/parser_bin.h"
#include "src/io/writer_bin.h"
#include "src/arg-handler.h"

#define MAX_OUT_FOLDER_PATH 256

int main(int argc, char* argv[])
{
    SA_DynamicArray* films = NULL;
    char out_folder_path[MAX_OUT_FOLDER_PATH] = "out/data.bin";

    SA_init();

    films = SA_dynarray_create_size_hint(Film, EXPECTED_FILM_NUMBERS);
    if(films == NULL)
    {
        goto EXIT_LBL;
    }

    for (int i = 1; i < argc; i++)
    {
        if(SA_strcmp(argv[i], "-o") == 0)
        {
            i++;
            if(i < argc)
            {
                SA_strncpy(out_folder_path, argv[i], MAX_OUT_FOLDER_PATH);
            }
        }
        else if(read_movie_file(films, argv[i]) != 0)
        {
            goto EXIT_LBL;
        }
    }
    
    write_films(out_folder_path, films);

EXIT_LBL:
    films_list_free(&films);

    SA_destroy();
}
