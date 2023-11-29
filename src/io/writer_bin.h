#ifndef WRITER_BIN_H
#define WRITER_BIN_H

#include <SA/memory/dynamic_array.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "src/data_structs.h"

int write_films(const char* out_filename, SA_DynamicArray* films);

#endif