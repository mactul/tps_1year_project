#include <stdio.h>
#include "file-io.h"
#include <stdint.h>

int read_movie_file(const char* filename)
{
    int error_code = 0;
    
    FILE* fd = fopen(filename, "r");
    if (fd == NULL)
    {
        goto Quit;
    }

    char buf[1024];
    uint64_t filmID;
    uint64_t userID;
    uint8_t rating;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    fscanf(fd, "%lu:", &filmID);
    printf("%lu\n", filmID);
    while (fgets(buf, 1000, fd))
    {
        sscanf(buf, "%lu,%hhu,%hu-%hhu-%hhu", &userID, &rating, &year, &month, &day);
        printf("%lu\n", userID);
        printf("%hhu\n", rating);
        printf("%hu\n", year);
        printf("%hhu\n", month);
        printf("%hhu\n", day);
    }

    Quit:
        if (fd != NULL)
        {
            fclose(fd);
        }
        return error_code;
}