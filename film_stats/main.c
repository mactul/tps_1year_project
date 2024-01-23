#include <stdio.h>
#include <SA/SA.h>
#include <signal.h>
#include "film_stats/calculate_stats.h"
#include "film_stats/arg-handler.h"
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/stats_io/writer_stats.h"
#include "src/recommendation/recommendation.h"
#include "src/signal_handler.h"
#include "src/data_structs.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAX_FILENAME_SIZE 256

/// @brief Create and write stats for all the movies
/// @param films Array containing all the movies
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param filter_options Pointer to a structure containing all the filters to use
/// @return
/// * RETURN_CODE_OK if everything went correctly
/// * RETURN_CODE_SIGNAL_ABORT if the program was interrupted
static enum RETURN_CODE create_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const StatsArguments* filter_options)
{
    int exit_code = RETURN_CODE_OK;
    SA_DynamicArray* film_stats = calculate_all_stats(films, reviewers, filter_options);

    if (film_stats == NULL)
    {
        exit_code = RETURN_CODE_SIGNAL_ABORT;
        goto FREE;
    }

    long last_slash = -1;
    char* out_folder = (char*) SA_malloc((SA_strlen(filter_options->out_file_path) + 1) * sizeof(char));
    SA_strcpy(out_folder, filter_options->out_file_path);

    for (int i = 0; i < SA_strlen(filter_options->out_file_path); i++)
    {
        if (out_folder[i] == '/')
        {
            last_slash = i;
        }
    }
    if (last_slash != -1)
    {
        out_folder[last_slash] = '\0';
        SA_recursive_mkdir(out_folder);
    }

    SA_free(&out_folder);

    write_stats(filter_options->out_file_path, film_stats);

FREE:
    SA_dynarray_free(&film_stats);
    return exit_code;
}

/// @brief The program generates statistics from a binary file and displays the best matches
/// @return 
/// * RETURN_CODE_OK if everything went correctly
/// * RETURN_CODE_ERROR_MEMORY if there was a memory allocation error
/// * RETURN_CODE_ERROR_FILE if the binary file doesn't exist
/// * RETURN_CODE_ERROR_ARGUMENTS if the command line arguments are incorrect
/// * RETURN_CODE_SIGNAL_ABORT if the user cancelled the program
int main(int argc, char* argv[])
{
    int server_socket = 0;
    int client_socket = 0;

    signal(SIGINT, sigint_handler);

    int exit_code = RETURN_CODE_OK;

    SA_DynamicArray* films = NULL;
    SA_DynamicArray* reviewers = NULL;
    StatsArguments args_structure;
    int index_remaining;
    FILE* file = NULL;

    SA_init();

    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        exit_code = RETURN_CODE_ERROR_ARGUMENTS;
        goto EXIT_LBL;
    }

    file = fopen(args_structure.in_file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Data file has not been generated, please run film_parser\n");
        exit_code = RETURN_CODE_ERROR_FILE;
        goto EXIT_LBL;
    }
    films = read_all_films(file);
    if (films == NULL)
    {
        exit_code = RETURN_CODE_ERROR_MEMORY;
        goto EXIT_LBL;
    }
    reviewers = read_all_reviewers(file);
    if(reviewers == NULL)
    {
        exit_code = RETURN_CODE_ERROR_MEMORY;
        goto EXIT_LBL;
    }
    fclose(file);
    file = NULL;

    if (args_structure.use_pipe == SA_TRUE)
    {
        // Execution was started using film_gui
        // Handle this
        goto EXIT_LBL;
    }

    exit_code = create_stats(films, reviewers, &args_structure);

EXIT_LBL:
    if(file != NULL)
    {
        fclose(file);
    }
    films_list_free(&films);
    SA_dynarray_free(&reviewers);

    SA_destroy();

    switch(exit_code)
    {
        case RETURN_CODE_ERROR_MEMORY:
            SA_print_error("Memory error\n");
            break;
        case RETURN_CODE_ERROR_FILE:
            SA_print_error("File I/O error\n");
            break;
        case RETURN_CODE_ERROR_ARGUMENTS:
            SA_print_error("Invalid arguments\n");
            break;
        case RETURN_CODE_SIGNAL_ABORT:
            SA_print_error("Stopped by user\n");
            break;
        default:
            break;
    }
    return exit_code;
}