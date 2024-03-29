#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <SA/memory/dynamic_array.h>

#define NUMBER_OF_YEARS_LOGGED_IN_STATS 10

#define YEARS_OFFSET 1850

#define EXPECTED_REVIEWER_NUMBERS 500000
#define EXPECTED_MAX_USER_ID 2700000
#define EXPECTED_FILM_NUMBERS 18000
#define EXPECTED_RATINGS_PER_FILM_NUMBER 256
#define MAX_FILM_NAME_SIZE 256

#define DEFAULT_DATA_FOLDER "data/"
#define DEFAULT_FILMS_DATA_FILE DEFAULT_DATA_FOLDER "film_data.bin"
#define DEFAULT_FILMS_STATS_FILE DEFAULT_DATA_FOLDER "film_stats.bin"

typedef struct _reviewer {
    uint16_t rate_count;
    uint16_t avg_note;
} Reviewer;

typedef struct _film {
    uint32_t film_id;
    uint32_t rating_count;
    SA_DynamicArray* ratings;
} Film;

typedef struct _film_info {
    uint16_t year;
    char name[MAX_FILM_NAME_SIZE];
} FilmInfo;

typedef struct _rating {
    uint32_t user_id;
    uint8_t note;
    uint8_t day;
    uint8_t month;
    uint8_t offseted_year;
} Rating;

typedef struct _stats_arguments {
    int32_t min_reviews;
    SA_bool use_pipe;
    const char* liked_films_filepath;
    const char* limit;
    const char* only_reviewers;
    const char* bad_reviewers;
    const char* out_file_path;
    const char* in_file_path;
} StatsArguments;

typedef struct _parser_arguments {
    const char* out_file_path;
} ParserArguments;

typedef struct _gui_arguments {
    const char* stats_bin_file;
} GuiArguments;

typedef struct _film_stats {
    uint32_t film_id;
    float recommendation;
    uint32_t max_year;
    float mean_rating_over_years[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    uint32_t kept_rating_count_over_years[NUMBER_OF_YEARS_LOGGED_IN_STATS];
} FilmStats;

enum RETURN_CODE {
    RETURN_CODE_OK,
    RETURN_CODE_ERROR_MEMORY,
    RETURN_CODE_ERROR_FILE,
    RETURN_CODE_ERROR_ARGUMENTS,
    RETURN_CODE_SIGNAL_ABORT
};

void films_list_free(SA_DynamicArray** films);

#endif