#include <stdio.h>
#include "file-io.h"

int main()
{
    for (int i = 0; i < 100; i++)
    {
        read_movie_file("download/training_set/mv_0000001.txt");
    }
}