#ifndef LIMIT_DATE_H
#define LIMIT_DATE_H

#include "src/data_structs.h"

void filter_date_from(Film* filtered, const Film* film_to_filter, uint8_t offsetted_year, uint8_t month, uint8_t day, SA_bool date_after);

#endif