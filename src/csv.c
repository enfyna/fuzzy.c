#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

#define LINE_SIZE 1024

void csv_print(Csv* csv)
{
    assert(csv != NULL);
    for (size_t i = 0; i < csv->title_count; i++) {
        printf("%7s", csv->titles[i]);
        if (i < csv->title_count - 1) {
            printf(", ");
        }
    }
    printf("\n");
    for (size_t i = 0; i < csv->data_count; i++) {
        for (size_t j = 0; j < csv->title_count; j++) {
            printf("% 7.2lf", csv->datas[i][j]);
            if (j < csv->title_count - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }
}

static size_t csv_count_title(FILE* fp)
{
    fpos_t pos;
    fgetpos(fp, &pos);

    size_t count = 0;
    char buf[LINE_SIZE] = { 0 };
    char* line = fgets(buf, LINE_SIZE, fp);
    char *token, *delim = ",\r\n";
    for (
        token = strtok(line, delim);
        token != NULL;
        token = strtok(NULL, delim), count++) { }
    fsetpos(fp, &pos);
    return count;
}

static size_t csv_count_line(FILE* fp)
{
    fpos_t pos;
    fgetpos(fp, &pos);

    size_t count = 0;
    char buf[LINE_SIZE] = { 0 };
    while (true) {
        char* line = fgets(buf, LINE_SIZE, fp);
        if (line == NULL) {
            break;
        }
        count++;
    }
    fsetpos(fp, &pos);
    return count;
}

Csv* csv_alloc_read_file_until(const char* csv_name, size_t max_read_line_data)
{
    FILE* f = fopen(csv_name, "r");
    assert(f != NULL && "[ERROR] Cant open file");

    size_t line_count = csv_count_line(f);
    if (max_read_line_data > 0 && max_read_line_data + 1 < line_count) {
        line_count = max_read_line_data + 1;
    }
    size_t title_count = csv_count_title(f);
    printf("[CSV] Title Count: %zu, Line Count: %zu\n", title_count, line_count);
    assert(title_count > 0);
    assert(line_count > 0);

    Csv* c = malloc(sizeof(Csv));
    c->data_count = line_count - 1;
    c->title_count = title_count;

    c->datas = malloc(sizeof(double*) * c->data_count);
    for (size_t i = 0; i < c->data_count; i++) {
        c->datas[i] = malloc(sizeof(double) * c->title_count);
    }
    c->titles = malloc(c->title_count * sizeof(char*));

    char buf[LINE_SIZE] = { 0 };
    const char* delim = ",\r\n";
    char *token, *line = fgets(buf, LINE_SIZE, f);
    size_t idx = 0;
    for (token = strtok(line, delim); token != NULL; idx++, token = strtok(NULL, delim)) {
        c->titles[idx] = strdup(token);
        // printf("%zu:%s\n", idx, c->titles[idx]);
    }
    assert(idx == title_count);
    for (size_t i = 0; i < line_count - 1; i++) {
        line = fgets(buf, LINE_SIZE, f);
        assert(line != NULL && "[ERROR] line_count is wrong!");

        idx = 0;
        for (token = strtok(line, delim); token != NULL; idx++, token = strtok(NULL, delim)) {
            c->datas[i][idx] = atof(token);
            // printf("%zu|%zu: %lf\n", i, idx, c->datas[i][idx]);
        }
        assert(idx == title_count && "[ERROR] Incomplete data!");
        assert(i < line_count - 1 && "[ERROR] Excessive data!");
        // printf("\n");
    }

    fclose(f);
    return c;
}
