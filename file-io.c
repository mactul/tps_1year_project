#include <stdbool.h>
#include <stdio.h>
#include "file-io.h"

bool read_movie_file(const char* filename)
{
    bool error_code = 0;
    FILE* fd = fopen(filename, "r");
    if (fd == NULL) goto Quit;

    

    Quit:
        fclose(fd);
        return error_code;
}