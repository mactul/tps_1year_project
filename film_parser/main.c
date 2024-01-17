#include <stdio.h>
#include <SA/SA.h>
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/arg-handler.h"
#include "src/signal_handler.h"

#define MAX_OUT_FOLDER_PATH 256

volatile sig_atomic_t _interruption_requested;

/// @brief This program goes through all the specified text files to create a binary file
/// @return 
/// * 0 if everything went correctly
/// * 1 if there was a memory allocation error
/// * 2 if the movie file doesn't exist
/// * 3 if the command line arguments are incorrect
/// * 4 if the user cancelled the program
int main(int argc, char* argv[])
{
    int exit_code = 0;
    _interruption_requested = 0;
    signal(SIGINT, sigint_handler);

    SA_DynamicArray* films = NULL;
    char out_folder_path[MAX_OUT_FOLDER_PATH] = DEFAULT_FILMS_DATA_FILE;

    SA_init();

    films = SA_dynarray_create_size_hint(Film, EXPECTED_FILM_NUMBERS);
    if(films == NULL)
    {
        exit_code = 1;
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
                exit_code = 3;
                goto EXIT_LBL;
            }
        }
        else if(read_movie_file(films, argv[i]) != 0)
        {
            exit_code = 2;
            goto EXIT_LBL;
        }
    }

    if (i < argc - 1)
    {
        exit_code = 4;
        printf("\nParsing aborted, file untouched\n");
        goto EXIT_LBL;
    }

    write_films(out_folder_path, films);

EXIT_LBL:
    films_list_free(&films);

    SA_destroy();
    return exit_code;
}
