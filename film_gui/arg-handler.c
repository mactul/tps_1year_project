#include <stdio.h>
#include <unistd.h>
#include <SA/SA.h>
#include "arg-handler.h"

/// @brief Print command-line options of this program
static void print_usage()
{
    puts(
        "film_gui, version 0.0.1, Macéo TULOUP & Valentin FOULON\n"
        "Usage: ./film_gui [options] <movie_titles_file>\n\n"
        "Options:\n"
        "-h\t\t\t\tDisplay this help\n"
        "-i IN_STATS_BIN_FILE_PATH\tAlternative stats binary file path"
    );
}

/// @brief Parse the command-line arguments
/// @param argc Number of arguments passed
/// @param argv Array of string arguments
/// @param args_structure Structure containing all the flags
/// @param arg_rest Pointer to a value that will be updated to be the index of the first non-flag string in argv (as getopt reorders all the arguments)
/// @return false if the arguments passed to the program are incorrect, else true
bool parse_args(int argc, char* argv[], GuiArguments* args_structure, int* arg_rest)
{
    args_structure->stats_bin_file = DEFAULT_FILMS_STATS_FILE;

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:h")) != -1)
    {
        switch(c)
        {
            case 'i':
                args_structure->stats_bin_file = optarg;
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

    if(optind == argc)
    {
        SA_print_error("You must specify the file path for the movie_titles.txt file\n\n");
        print_usage();
        return false;
    }
    if (optind < argc - 1)
    {
        SA_print_error("Too many files passed\n\n");
        print_usage();
        return false;
    }
    return true;
}
