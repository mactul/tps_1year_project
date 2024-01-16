#include <stdio.h>
#include <unistd.h>
#include <SA/SA.h>
#include "arg-handler.h"

void print_usage(void)
{
    puts(
        "film_stats, version 0.0.1, Macéo TULOUP & Valentin FOULON\n"
        "Usage: ./film_stats [options] <data_binary_file>\n\n"
        "Options:\n"
        "-h\t\t\tDisplay this help\n"
        "-f FOLDER\t\tDefines the output folder for the stats.bin file\n"
        "-l DATE_LIMIT\t\tOnly take into account reviews created before DATE_LIMIT\n"
        "-c \"X, Y\"\t\tOnly take into account the opinions of reviewers X, Y\n"
        "-b \"X, Y\"\t\tNot taking into account the opinions of reviewers X, Y\n"
        "-e MIN\t\tOnly take into account reviews from reviewers who have seen more than MIN films\n"
        "-r FILEPATH\t\tMake recommendations with a file containing the list of film IDs you like\n"
        //"-t TIMEOUT\t\tDefine TIMEOUT as the maximum execution time in seconds\n"
        "-g\t\t\tUse the graphical user interface\n"
    );
}

bool parse_args(int argc, char* argv[], Arguments* args_structure, int* arg_rest)
{
    args_structure->bad_reviewers = NULL;
    args_structure->limit = NULL;
    args_structure->min_reviews = -1;
    args_structure->only_reviewers = NULL;
    args_structure->output_folder = NULL;
    args_structure->timeout_milli = -1;
    args_structure->liked_films_filepath = NULL;
    args_structure->use_graphics = SA_FALSE;

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "f:l:s:c:b:e:t:r:gh")) != -1)
    {
        switch(c)
        {
            case 'f':
                args_structure->output_folder = optarg;
                break;
            case 'l':
                args_structure->limit = optarg;
                break;
            case 'c':
                args_structure->only_reviewers = optarg;
                break;
            case 'b':
                args_structure->bad_reviewers = optarg;
                break;
            case 'r':
                args_structure->liked_films_filepath = optarg;
                break;
            case 'e':
                args_structure->min_reviews = SA_str_to_uint64(optarg);
                if (args_structure->min_reviews == 0 && SA_get_last_error() == SA_ERROR_NAN)
                {
                    SA_print_error("Invalid minimum reviews\n");
                    print_usage();
                    return false;
                }
                break;
            case 't':
                args_structure->timeout_milli = SA_str_to_uint64(optarg);
                if (args_structure->timeout_milli == 0)
                {
                    SA_print_error("Invalid timeout\n");
                    print_usage();
                    return false;
                }
                break;
            case 'g':
                args_structure->use_graphics = SA_TRUE;
                break;
            case 'h':
                print_usage();
                return false;
            case '?':
                SA_print_error("Invalid command line options\n");
                print_usage();
                return false;
        }
    }
    *arg_rest = optind;
    if (optind < argc-1)
    {
        SA_print_error("Trop de fichiers fournis\n");
        print_usage();
        return false;
    }
    return true;
}