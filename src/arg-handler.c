#include <stdio.h>
#include <unistd.h>
#include <SA/SA.h>
#include "arg-handler.h"

void print_usage(void)
{
    puts(
        "Prog, version 0.0.1, Macéo TULOUP & Valentin FOULON\n"
        "Utilisation: ./prog [options] <fichiers>\n\n"
        "Options:\n"
        "-h\t\t\tAfficher cette aide\n"
        "-f DOSSIER\t\tDéfinir DOSSIER comme dossier de sortie\n"
        "-l LIMITE\t\tNe prendre en compte que les avis crées avant LIMITE\n"
        "-s FILM_ID\t\tDonner des statistiques sur le film avec l'identifiant FILM_ID\n"
        "-c \"X, Y\"\t\tNe prendre en compte que les avis des clients X, Y\n"
        "-b \"X, Y\"\t\tNe pas prendre en compte les avis des clients X, Y\n"
        "-e MIN\t\t\tNe prendre en compte que les avis de clients ayant vu plus de MIN films\n"
        "-t TIMEOUT\t\tDéfinir TIMEOUT comme temps d'exécution maximum"
    );
}

bool parse_args(int argc, char* argv[], Arguments* args_structure, int* arg_rest)
{
    args_structure->bad_reviewers = NULL;
    args_structure->film_id = -1;
    args_structure->limit = -1;
    args_structure->min_reviews = -1;
    args_structure->only_reviewers = NULL;
    args_structure->output_folder = NULL;
    args_structure->timeout_milli = -1;

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "f:l:s:c:b:e:t:h")) != -1)
    {
        int i = 0;
        while(optarg[i] == ' ')
        {
            i++;
        }
        switch(c)
        {
            case 'f':
                args_structure->output_folder = optarg+i;
                break;
            case 'l':
                args_structure->limit = SA_str_to_uint64(optarg+i);
                if (args_structure->limit == 0)
                {
                    SA_print_error("Invalid limit\n");
                    print_usage();
                    return false;
                }
                break;
            case 's':
                args_structure->film_id = SA_str_to_uint64(optarg+i);
                if (args_structure->film_id == 0 && SA_get_last_error() == SA_ERROR_NAN)
                {
                    SA_print_error("Invalid film ID\n");
                    print_usage();
                    return false;
                }
                break;
            case 'c':
                args_structure->only_reviewers = optarg+i;
                break;
            case 'b':
                args_structure->bad_reviewers = optarg+i;
                break;
            case 'e':
                args_structure->min_reviews = SA_str_to_uint64(optarg+i);
                if (args_structure->min_reviews == 0 && SA_get_last_error() == SA_ERROR_NAN)
                {
                    SA_print_error("Invalid minimum reviews\n");
                    print_usage();
                    return false;
                }
                break;
            case 't':
                args_structure->timeout_milli = SA_str_to_uint64(optarg+i);
                if (args_structure->timeout_milli == 0)
                {
                    SA_print_error("Invalid timeout\n");
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
    if (optind == argc)
    {
        SA_print_error("Pas de fichiers fournis\n");
        print_usage();
        return false;
    }
    return true;
}