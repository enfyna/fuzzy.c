#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "fuzzy.h"

#define data_anchor_ratio 0
#define data_trans_range 1
#define data_node_density 2
#define data_iterations 3
#define data_ale 4
#define data_sd_ale 5

const char* title[] = {
    "Very Low %s",
    "Low %s",
    "Med %s",
    "High %s",
    "Very High %s",
};
size_t title_count = sizeof title / sizeof title[0];
size_t class_count = sizeof title / sizeof title[0];

int main(void)
{

    enum { read_data = 100 };
    Csv* csv = csv_alloc_read_file_until("res/mcs_ds_edited_iter_shuffled.csv",
        read_data);
    assert(read_data == csv->data_count);

    Rule* rules[] = {
        rule_alloc(2,
            rule_lit(R_LOW, data_anchor_ratio, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(2,
            rule_lit(R_HIGH, data_node_density, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(2,
            rule_lit(R_VERY_LOW, data_trans_range, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(2,
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(2,
            rule_lit(R_LOW, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_VERY_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_VERY_LOW, data_trans_range, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_MED, data_anchor_ratio, R_AND),
            rule_lit(R_LOW, data_trans_range, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_HIGH, data_node_density, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_LOW, data_node_density, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_HIGH, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_trans_range, R_EQUALS),
            rule_lit(R_VERY_LOW, data_ale, R_STOP)),
        rule_alloc(3,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_trans_range, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_MED, data_trans_range, R_AND),
            rule_lit(R_HIGH, data_node_density, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_HIGH, data_anchor_ratio, R_AND),
            rule_lit(R_LOW, data_node_density, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_MED, data_anchor_ratio, R_AND),
            rule_lit(R_MED, data_trans_range, R_AND),
            rule_lit(R_MED, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_MED, data_anchor_ratio, R_AND),
            rule_lit(R_MED, data_node_density, R_AND),
            rule_lit(R_MED, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(4,
            rule_lit(R_MED, data_trans_range, R_AND),
            rule_lit(R_MED, data_node_density, R_AND),
            rule_lit(R_MED, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(5,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_trans_range, R_AND),
            rule_lit(R_HIGH, data_node_density, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
        rule_alloc(5,
            rule_lit(R_HIGH, data_anchor_ratio, R_AND),
            rule_lit(R_LOW, data_trans_range, R_AND),
            rule_lit(R_LOW, data_node_density, R_AND),
            rule_lit(R_LOW, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(5,
            rule_lit(R_MED, data_anchor_ratio, R_AND),
            rule_lit(R_MED, data_trans_range, R_AND),
            rule_lit(R_MED, data_node_density, R_OR),
            rule_lit(R_MED, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(5,
            rule_lit(R_LOW, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_trans_range, R_AND),
            rule_lit(R_LOW, data_node_density, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_MED, data_ale, R_STOP)),
        rule_alloc(5,
            rule_lit(R_HIGH, data_anchor_ratio, R_AND),
            rule_lit(R_HIGH, data_trans_range, R_AND),
            rule_lit(R_LOW, data_node_density, R_OR),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
    };
    enum { rules_count = sizeof rules / sizeof rules[0] };

    Fuzzy* fs[] = {
        // data_anchor_ratio
        fuzzy_alloc(5, 10.0, 30.0,
            fz_trimf(10, 10, 25),
            fz_trimf(10, 20, 30),
            fz_trimf(10, 20, 30),
            fz_trimf(10, 20, 30),
            fz_trimf(25, 30, 31)),
        // data_trans_range
        fuzzy_alloc(5, 15.0, 25.0,
            fz_gauss(15, 1),
            fz_gauss(18, 1),
            fz_gauss(20, 1),
            fz_gauss(22, 1),
            fz_gauss(25, 1)),
        // data_node_density
        fuzzy_alloc(5, 100.0, 300.0,
            fz_gauss(100, 25),
            fz_gauss(150, 25),
            fz_gauss(200, 25),
            fz_gauss(250, 25),
            fz_gauss(300, 25)),
        // data_iterations
        fuzzy_alloc(5, 15.0, 100.0,
            fz_gauss(15, 10),
            fz_gauss(30, 10),
            fz_gauss(50, 10),
            fz_gauss(70, 10),
            fz_gauss(100, 10)),
        // data_ale
        fuzzy_alloc(3, 0.0, 3.0,
            fz_gauss(0, 0.5),
            fz_gauss(1, 0.5),
            fz_gauss(2, 0.5))
    };
    enum { fs_count = sizeof fs / sizeof fs[0] };

    // Memberships
    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].count = class_count + 1;
        ms[i].items = malloc(sizeof(double) * (class_count + 1));
    }

    Array res[read_data];
    for (size_t i = 0; i < read_data; i++) {
        res[i].count = fs[data_ale]->count;
        res[i].items = malloc(sizeof(double) * fs[data_ale]->count);
    }

    double results[read_data] = { 0 };

    double total_diff_ape = 0;
    double total_diff_mae = 0;
    double total_diff_rmse = 0;

    for (size_t line = 0; line < read_data; line++) { // change 1 to  csv->data_count
        printf("forwarding for data %zu:\n", line + 1);
        for (size_t class = 0; class < class_count - 1; class++) {
            ms[class].count = fs[class]->count;
            fuzzy_forward(ms[class], fs[class], csv->datas[line][class]);

            printf("%s:\n\t", csv->titles[class]);
            for (size_t j = 0; j < ms[class].count; j++) {
                printf("%.2f, ", ms[class].items[j]);
            }
            printf("\n");
        }

        rule_forward(res[line], ms, rules, rules_count);

        double defuzz = fuzzy_defuzzify(fs[data_ale], res[line]);
        results[line] = defuzz;

        printf("res: ");
        for (size_t i = 0; i < res[line].count; i++) {
            printf("%.2f, ", res[line].items[i]);
        }
        printf("\n");

        double actual = csv->datas[line][data_ale];
        double diff = actual - results[line];

        if (actual != 0) {
            total_diff_ape += fabs(diff) / (actual) * 100.0;
        }
        total_diff_mae += diff;
        total_diff_rmse += pow(diff, 2);

        printf("guess ale: %.2f | real ale: %.2f | diff: %.2f\n", results[line],
            actual, diff);
        printf("=============\n");
    }

    double mape = total_diff_ape / read_data;
    double mae = total_diff_mae / read_data;
    double rmse = sqrt(total_diff_rmse / read_data);
    printf("mae: %.2f | rmse: %.2f | mape: %.2f\n", mae, rmse, mape);
    printf("=============\n");

    return 0;
}
