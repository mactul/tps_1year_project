#include <stdio.h>
#include <unistd.h>
#include <SA/SA.h>
#include "arg-handler.h"

/// @brief Print command-line options of this program
void print_usage()
{
    puts(
        "film_stats, version 0.0.1, Macéo TULOUP & Valentin FOULON\n"
        "Usage: ./film_stats [options]\n\n"
        "Options:\n"
        "-h\t\t\t\tDisplay this help\n"
        "-o OUT_STATS_BIN_FILE_PATH\tAlternative stats binary file path\n"
        "-l DATE_LIMIT\t\t\tOnly take into account reviews created before DATE_LIMIT\n"
        "-c \"X, Y\"\t\t\tOnly take into account the opinions of reviewers X, Y\n"
        "-b \"X, Y\"\t\t\tNot taking into account the opinions of reviewers X, Y\n"
        "-e MIN\t\t\t\tOnly take into account reviews from reviewers who have seen more than MIN films\n"
        "-r LIKED_FILMS_TEXT_FILE_PATH\tMake recommendations with a file containing the list of film IDs you like\n"
        "-i IN_DATA_BIN_FILE_PATH\tAlternative data binary file path"
    );
}

/// @brief Parse the command-line arguments
/// @param argc Number of arguments passed
/// @param argv Array of string arguments
/// @param args_structure Structure containing all the flags
/// @param arg_rest Pointer to a value that will be updated to be the index of the first non-flag string in argv (as getopt reorders all the arguments)
/// @return false if the arguments passed to the program are incorrect, else true
bool parse_args(int argc, char* argv[], StatsArguments* args_structure, int* arg_rest)
{
    args_structure->bad_reviewers = NULL;
    args_structure->limit = NULL;
    args_structure->min_reviews = -1;
    args_structure->only_reviewers = NULL;
    args_structure->out_file_path = DEFAULT_FILMS_STATS_FILE;
    args_structure->liked_films_filepath = NULL;
    args_structure->in_file_path = DEFAULT_FILMS_DATA_FILE;

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "o:l:s:c:b:e:r:gh")) != -1)
    {
        switch(c)
        {
            case 'o':
                args_structure->out_file_path = optarg;
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

    if(optind != argc)
    {
        SA_print_error("film_stats \n\n");
        print_usage();
        return false;
    }
    return true;
}
