#ifndef PARSE_LIKED_FILMS_H
#define PARSE_LIKED_FILMS_H

#include "src/data_structs.h"

SA_DynamicArray* parse_liked_films(const char* liked_films_filepath, const SA_DynamicArray* films);

#endif