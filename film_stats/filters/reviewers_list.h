#ifndef REVIEWERS_LIST_H
#define REVIEWERS_LIST_H

#include "src/data_structs.h"

void filter_reviewers(Film* film_filtered, const Film* film_to_filter, const char* reviewers, SA_bool whitelist_mode);

#endif