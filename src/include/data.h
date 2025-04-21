#pragma once

#include "fuzzy.h"

#define DATA_COUNT 107

#define data_anchor_ratio 0
#define data_trans_range 1
#define data_node_density 2
#define data_iterations 3
#define data_ale 4
#define data_sd_ale 5

typedef struct {
    char** titles;
    size_t title_count;
    double** datas;
    size_t data_count;
} Csv;

#define rule_first_data 1
#define rule_first_membership 2
#define rule_second_data 3
#define rule_second_membership 4
#define rule_third_data 5
#define rule_third_membership 6

enum RuleOp {
    R_AND,
    R_OR,
};

enum RuleClass {
    R_VERY_LOW,
    R_LOW,
    R_MED,
    R_HIGH,
    R_VERY_HIGH,
};

typedef struct {
    int key;
    double value;
} RuleLit;

typedef struct {
    RuleLit literals[4];
    size_t literal_count;
    enum RuleOp op[3];
    RuleLit expected;
} Rule;

#define csv_alloc_read_file(file_name) csv_alloc_read_file_until(file_name, 0)
Csv* csv_alloc_read_file_until(const char* csv_name, size_t max_read_line_data);
void csv_print(Csv* csv);

void rule_forward(memberships ms, fuzzy* fz, Rule* rule, double value);
