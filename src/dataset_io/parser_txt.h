#ifndef FILE_IO_H
#define FILE_IO_H

#include "src/data_structs.h"

int read_movie_file(SA_DynamicArray* films, const char* filename);
SA_DynamicArray* get_films_infos(const char* movie_titles_filepath);

#endif