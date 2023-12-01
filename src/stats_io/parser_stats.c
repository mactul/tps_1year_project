#include "src/stats_io/parser_stats.h"
#include "src/data_structs.h"

SA_DynamicArray* read_stats(FILE* file)
{
    int error_code = 0;
    SA_DynamicArray* stats = NULL;

    if (file == NULL)
    {
        error_code = 1;
        goto QUIT;
    }

    uint64_t stat_count;

    if (fread(&stat_count, sizeof(stat_count), 1, file) <= 0)
    {
        error_code = 1;
        goto QUIT;
    }

    stats = SA_dynarray_create_size_hint(FilmStats, stat_count + 1);

    for (uint64_t i = 0; i < stat_count; i++)
    {
        FilmStats s;
        if (fread(&s, sizeof(s), 1, file) <= 0)
        {
            error_code = 1;
            goto QUIT;
        }
        SA_dynarray_append(FilmStats, stats, s);
    }

QUIT:
    if (!error_code)
    {
        return stats;
    }
    SA_dynarray_free(&stats);
    return NULL;
}