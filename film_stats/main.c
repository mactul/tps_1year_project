#include <stdio.h>
#include <SA/SA.h>
#include <signal.h>
#include "film_stats/calculate_stats.h"
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/stats_io/writer_stats.h"
#include "src/arg-handler.h"

#define MAX_OUT_FOLDER_PATH 256

void sigint_close_files(int signum __attribute__((unused)))
{
    printf("Attendez une minute, c'est bientôt fini\n");
    return;
}

static void create_stats(const SA_DynamicArray* films, const SA_DynamicArray* reviewers, const Arguments* filter_options)
{
    SA_DynamicArray* film_stats = calculate_all_stats(films, reviewers, filter_options);
    printf("out/stats.bin");
    write_stats("out/stats.bin", film_stats);
    SA_dynarray_free(&film_stats);
}

int main(int argc, char* argv[])
{
    //signal(SIGINT, sigint_close_files);

    SA_DynamicArray* films = NULL;
    SA_DynamicArray* reviewers = NULL;
    char out_folder_path[MAX_OUT_FOLDER_PATH] = "out/data.bin";
    Arguments args_structure;
    int index_remaining;

    SA_init();

    FILE* file = fopen(out_folder_path, "r");

    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        goto EXIT_LBL;
    }

    if(index_remaining < argc)
    {
        SA_strcpy(out_folder_path, argv[index_remaining]);
    }


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