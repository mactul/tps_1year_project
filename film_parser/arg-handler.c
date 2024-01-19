#include <stdio.h>
#include <unistd.h>
#include <SA/SA.h>
#include "arg-handler.h"

/// @brief Print command-line options of this program
void print_usage()
{
    puts(
        "film_parser, version 0.0.1, Macéo TULOUP & Valentin FOULON\n"
        "Usage: ./film_parser [options] <source_1.txt> <source_2.txt> ...\n\n"
        "Options:\n"
        "-h\t\t\t\tDisplay this help\n"
        "-o OUT_DATA_BIN_FILE_PATH\tSelect the binary output file path"
    );
}

/// @brief Parse the command-line arguments
/// @param argc Number of arguments passed
/// @param argv Array of string arguments
/// @param args_structure Structure containing all the flags
/// @param arg_rest Pointer to a value that will be updated to be the index of the first non-flag string in argv (as getopt reorders all the arguments)
/// @return false if the arguments passed to the program are incorrect, else true
bool parse_args(int argc, char* argv[], ParserArguments* args_structure, int* arg_rest)
{
    args_structure->out_file_path = NULL;

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "o:h")) != -1)
    {
        switch(c)
        {
            case 'o':
                args_structure->out_file_path = optarg;
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
        SA_print_error("You must provide at least one source text file\n\n");
        print_usage();
        return false;
    }
    return true;
}
