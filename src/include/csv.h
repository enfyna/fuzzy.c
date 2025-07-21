#pragma once

#include <stddef.h>

typedef struct {
    char** titles;
    size_t title_count;
    double** datas;
    size_t data_count;
} Csv;

#define csv_alloc_read_file(file_name) csv_alloc_read_file_until(file_name, 0)
Csv* csv_alloc_read_file_until(const char* csv_name, size_t max_read_line_data);
void csv_print(Csv* csv);
void csv_free(Csv* csv);
