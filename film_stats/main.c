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

#define MAX_FILENAME_SIZE 256

/// @brief Create and write stats for all the movies
/// @param films Array containing all the movies
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param filter_options Pointer to a structure containing all the filters to use
/// @param movie_title_filepath File path of the movie_titles.txt file
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
        SA_print_error("\nInterrupted by user\n\n");
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
/// * RETURN_CODE_ERROR_FILE_NOT_FOUND if the binary file doesn't exist
/// * RETURN_CODE_ERROR_ARGUMENTS if the command line arguments are incorrect
/// * RETURN_CODE_SIGNAL_ABORT if the user cancelled the program
int main(int argc, char* argv[])
{
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
        exit_code = RETURN_CODE_ERROR_FILE_NOT_FOUND;
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

    exit_code = create_stats(films, reviewers, &args_structure);

    printf("distance between Harry Potter II  & Harry Potter II : %f\n", distance_between_films(_SA_dynarray_get_element_ptr(films, 11443), _SA_dynarray_get_element_ptr(films, 11443), reviewers));
    printf("distance between Harry Potter II  & Harry Potter I  : %f\n", distance_between_films(_SA_dynarray_get_element_ptr(films, 11443), _SA_dynarray_get_element_ptr(films, 17627), reviewers));
    printf("distance between Harry Potter II  & Star Wars IV    : %f\n", distance_between_films(_SA_dynarray_get_element_ptr(films, 11443), _SA_dynarray_get_element_ptr(films, 16265), reviewers));
    printf("distance between Star Wars V      & Star Wars IV    : %f\n", distance_between_films(_SA_dynarray_get_element_ptr(films, 5582), _SA_dynarray_get_element_ptr(films, 16265), reviewers));
    printf("distance between Fast and Furious & Amelie          : %f\n", distance_between_films(_SA_dynarray_get_element_ptr(films, 6844), _SA_dynarray_get_element_ptr(films, 6029), reviewers));

EXIT_LBL:
    if(file != NULL)
    {
        fclose(file);
    }
    films_list_free(&films);
    SA_dynarray_free(&reviewers);

    SA_destroy();

    return exit_code;
}