#include "src/stats_io/writer_stats.h"
#include "src/data_structs.h"
#include <stdio.h>

void write_stats(const char* stats_filename, SA_DynamicArray* stats)
{

    FILE* file = fopen(stats_filename, "wb");
    if (file == NULL)
    {
        goto QUIT;
    }

    uint64_t stat_count = SA_dynarray_size(stats);

    if (fwrite(&stat_count, sizeof(stat_count), 1, file) <= 0)
    {
        goto QUIT;
    }

    for (uint64_t i = 0; i < stat_count; i++)
    {
        FilmStats stat = SA_dynarray_get(FilmStats, stats, i);
        if (fwrite(&stat, sizeof(stat), 1, file) <= 0)
        {
            goto QUIT;
        }
    }

QUIT:
    if (file != NULL)
    {
        fclose(file);
    }
    return;
}