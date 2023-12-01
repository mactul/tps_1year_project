#include <stdio.h>
#include <SA/SA.h>
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/arg-handler.h"
#include "src/signal_handler.h"

#define MAX_OUT_FOLDER_PATH 256

volatile sig_atomic_t _interruption_requested;

int main(int argc, char* argv[])
{

    _interruption_requested = 0;
    signal(SIGINT, sigint_handler);

    SA_DynamicArray* films = NULL;
    char out_folder_path[MAX_OUT_FOLDER_PATH] = "out/data.bin";

    SA_init();

    films = SA_dynarray_create_size_hint(Film, EXPECTED_FILM_NUMBERS);
    if(films == NULL)
    {
        goto EXIT_LBL;
    }

    int i;

    for (i = 1; i < argc && !_interruption_requested; i++)
    {
        if(SA_strcmp(argv[i], "-o") == 0)
        {
            i++;
            if(i < argc)
            {
                SA_strncpy(out_folder_path, argv[i], MAX_OUT_FOLDER_PATH);
            }
            else
            {
                SA_print_error("-o requires an argument\n");
                goto EXIT_LBL;
            }
        }
        else if(read_movie_file(films, argv[i]) != 0)
        {
            goto EXIT_LBL;
        }
    }

    if (i < argc - 1)
    {
        printf("Interrupted by user\n");
        goto EXIT_LBL;
    }

    write_films(out_folder_path, films);

EXIT_LBL:
    films_list_free(&films);

    SA_destroy();
}
