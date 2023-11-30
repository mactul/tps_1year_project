#include <stdio.h>
#include <SA/SA.h>
#include "src/dataset_io/parser_txt.h"
#include "src/dataset_io/parser_bin.h"
#include "src/dataset_io/writer_bin.h"
#include "src/arg-handler.h"

#define MAX_OUT_FOLDER_PATH 256


int main(int argc, char* argv[])
{
    SA_DynamicArray* films = NULL;
    char out_folder_path[MAX_OUT_FOLDER_PATH] = "out/data.bin";
    Arguments args_structure;
    int index_remaining;

    SA_init();


    if (!parse_args(argc, argv, &args_structure, &index_remaining))
    {
        goto EXIT_LBL;
    }

    if(index_remaining < argc)
    {
        SA_strcpy(out_folder_path, argv[index_remaining]);
    }

    films = read_all_films(out_folder_path);
    if (films == NULL)
    {
        goto EXIT_LBL;
    }

EXIT_LBL:
    films_list_free(&films);

    SA_destroy();
}