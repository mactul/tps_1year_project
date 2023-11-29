#ifndef WRITER_BIN_H
#define WRITER_BIN_H

#include <SA/memory/dynamic_array.h>
#include "src/data_structs.h"

int write_films(const char* out_filename, SA_DynamicArray* films);

#endif