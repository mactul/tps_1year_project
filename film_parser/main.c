#include <stdio.h>
#include <SA/SA.h>
#include "film_parser/arg-handler.h"
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/signal_handler.h"
#include "src/data_structs.h"

#define MAX_OUT_FILE_PATH 256

volatile sig_atomic_t _interruption_requested;

/// @brief This program goes through all the specified text files to create a binary file
/// @return 
/// * RETURN_CODE_OK if everything went correctly
/// * RETURN_CODE_ERROR_MEMORY if there was a memory allocation error
/// * RETURN_CODE_ERROR_FILE_NOT_FOUND if the movie file doesn't exist
/// * RETURN_CODE_ERROR_ARGUMENTS if the command line arguments are incorrect
/// * RETURN_CODE_SIGNAL_ABORT if the user cancelled the program
int main(int argc, char* argv[])
{
    int exit_code = RETURN_CODE_OK;
    _interruption_requested = 0;
    signal(SIGINT, sigint_handler);

    SA_DynamicArray* films = NULL;
    char out_file_path[MAX_OUT_FILE_PATH] = DEFAULT_FILMS_DATA_FILE;

    SA_init();

    films = SA_dynarray_create_size_hint(Film, EXPECTED_FILM_NUMBERS);
    if(films == NULL)
    {
        exit_code = RETURN_CODE_ERROR_MEMORY;
        goto EXIT_LBL;
    }

    ParserArguments args_structure;
    int arg_rest;
    if (!parse_args(argc, argv, &args_structure, &arg_rest))
    {
        exit_code = RETURN_CODE_ERROR_ARGUMENTS;
        goto EXIT_LBL;
    }

    if (args_structure.out_file_path != NULL)
    {
        SA_strncpy(out_file_path, args_structure.out_file_path, MAX_OUT_FILE_PATH);
    }

    for (int i = arg_rest; i < argc && !_interruption_requested; i++)
    {
        if (read_movie_file(films, argv[i]) != 0)
        {
            exit_code = RETURN_CODE_ERROR_FILE_NOT_FOUND;
            goto EXIT_LBL;
        }
    }

    if (_interruption_requested)
    {
        exit_code = RETURN_CODE_SIGNAL_ABORT;
        goto EXIT_LBL;
    }

    write_films(out_file_path, films);

EXIT_LBL:
    films_list_free(&films);

    SA_destroy();
    return exit_code;
}
