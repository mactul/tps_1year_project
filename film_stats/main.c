#include <stdio.h>
#include <SA/SA.h>
#include <signal.h>
#include "film_stats/calculate_stats.h"
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/stats_io/writer_stats.h"
#include "src/arg-handler.h"

#define MAX_OUT_FILE_PATH 256

void sigint_close_files(int signum __attribute__((unused)))
{
    printf("Attendez une minute, c'est bientôt fini\n");
    return;
}

static void create_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const Arguments* filter_options)
{
    SA_DynamicArray* film_stats = calculate_all_stats(films, reviewers, filter_options);
    char out_file[MAX_OUT_FILE_PATH] = "";
    const char* default_folder = "out/";
    uint8_t folder_length = SA_strlen(default_folder);

    if (filter_options->output_folder != NULL)
    {
        folder_length = (uint8_t) SA_strlen(filter_options->output_folder);
        SA_strncpy(out_file, filter_options->output_folder, MAX_OUT_FILE_PATH);
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

    SA_strcpy(&out_file[folder_length], "stats.bin");

    printf("%s", out_file);
    write_stats(out_file, film_stats);
    SA_dynarray_free(&film_stats);
}

int main(int argc, char* argv[])
{
    //signal(SIGINT, sigint_close_files);

    SA_DynamicArray* films = NULL;
    SA_DynamicArray* reviewers = NULL;
    char out_file_path[MAX_OUT_FILE_PATH] = "out/data.bin";
    Arguments args_structure;
    int index_remaining;

    SA_init();

    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        goto EXIT_LBL;
    }

    if(index_remaining < argc)
    {
        SA_strncpy(out_file_path, argv[index_remaining], MAX_OUT_FILE_PATH);
    }

    FILE* file = fopen(out_file_path, "r");

    if (file == NULL)
    {
        goto EXIT_LBL;
    }
    films = read_all_films(file);
    if (films == NULL)
    {
        goto EXIT_LBL;
    }
    reviewers = read_all_reviewers(file);
    if(reviewers == NULL)
    {
        goto EXIT_LBL;
    }
    fclose(file);
    file = NULL;

    create_stats(films, reviewers, &args_structure);


EXIT_LBL:
    if(file != NULL)
    {
        fclose(file);
    }
    films_list_free(&films);
    SA_dynarray_free(&reviewers);

    SA_destroy();

}