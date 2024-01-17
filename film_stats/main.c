#include <stdio.h>
#include <SA/SA.h>
#include <signal.h>
#include "film_stats/calculate_stats.h"
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/stats_io/writer_stats.h"
#include "src/arg-handler.h"
#include "src/recommendation/recommendation.h"

#define MAX_FILENAME_SIZE 256

/// @brief Signal handler for Ctrl+C
/// @param signum Signal number
void sigint_close_files(int signum __attribute__((unused)))
{
    printf("Attendez une minute, c'est bientôt fini\n");
    return;
}

/// @brief Create and write stats for all the movies
/// @param films Array containing all the movies
/// @param reviewers Array of structures containing the number of ratings and average rating for each user
/// @param filter_options Pointer to a structure containing all the filters to use
/// @param movie_title_filepath File path of the movie_titles.txt file
static void create_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const Arguments* filter_options, const char* movie_title_filepath)
{
    SA_DynamicArray* film_stats = calculate_all_stats(films, reviewers, filter_options);
    char out_file[MAX_FILENAME_SIZE] = "";
    const char* default_folder = DEFAULT_DATA_FOLDER;
    uint8_t folder_length = SA_strlen(default_folder);

    if (filter_options->output_folder != NULL)
    {
        folder_length = (uint8_t) SA_strlen(filter_options->output_folder);
        SA_strncpy(out_file, filter_options->output_folder, MAX_FILENAME_SIZE);
    }
    else
    {
        SA_strcpy(out_file, default_folder);
    }

    if (out_file[folder_length - 1] != '/')
    {
        folder_length++;
        out_file[folder_length - 1] = '/';
    }

    SA_DynamicArray* films_infos = get_films_infos(movie_title_filepath);

    if(films_infos == NULL)
    {
        SA_print_error("Impossible to get the film titles, please make sure the file path is good\n\n");
        goto FREE;
    }

    for(int i = 0; i < 40; i++)
    {
        FilmStats* fstats = _SA_dynarray_get_element_ptr(film_stats, i);
        FilmInfo info = SA_dynarray_get(FilmInfo, films_infos, fstats->film_id);
        printf("%d : %s\n", fstats->film_id, info.name);
    }

FREE:
    SA_dynarray_free(&films_infos);

    SA_recursive_mkdir(out_file);

    SA_strcpy(&out_file[folder_length], "film_stats.bin");

    write_stats(out_file, film_stats);
    SA_dynarray_free(&film_stats);
}

/// @brief The program generates statistics from a binary file and displays the best matches
/// @return 
/// * 0 if everything went correctly
/// * 1 if there was a memory allocation error
/// * 2 if the binary file doesn't exist
/// * 3 if the command line arguments are incorrect
/// * 4 if the user cancelled the program (not yet implemented)
int main(int argc, char* argv[])
{
    //signal(SIGINT, sigint_close_files);

    int exit_code = 0;

    SA_DynamicArray* films = NULL;
    SA_DynamicArray* reviewers = NULL;
    char movie_titles_file[MAX_FILENAME_SIZE];
    char in_file_path[MAX_FILENAME_SIZE] = DEFAULT_FILMS_DATA_FILE;
    Arguments args_structure;
    int index_remaining;
    FILE* file = NULL;

    SA_init();

    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        exit_code = 3;
        goto EXIT_LBL;
    }

    SA_strncpy(movie_titles_file, argv[index_remaining], MAX_FILENAME_SIZE);
    index_remaining++;

    if(index_remaining < argc)
    {
        SA_strncpy(in_file_path, argv[index_remaining], MAX_FILENAME_SIZE);
    }

    file = fopen(in_file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Data file has not been generated, please run film_parser\n");
        exit_code = 2;
        goto EXIT_LBL;
    }
    films = read_all_films(file);
    if (films == NULL)
    {
        exit_code = 1;
        goto EXIT_LBL;
    }
    reviewers = read_all_reviewers(file);
    if(reviewers == NULL)
    {
        exit_code = 1;
        goto EXIT_LBL;
    }
    fclose(file);
    file = NULL;

    create_stats(films, reviewers, &args_structure, movie_titles_file);

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

}