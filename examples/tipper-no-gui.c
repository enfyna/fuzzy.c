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

#define data_service 0
#define data_food 1
#define data_tip 2

int main(void)
{
    enum { read_data = 3 };
    Csv* csv = csv_alloc_read_file_until("res/tipper.csv", read_data);
    assert(read_data == csv->data_count);

    Fuzzy* fs[] = {
        fuzzy_alloc("Service", 3, 0, 10,
            fz_gauss("Bad", 0, 2),
            fz_gauss("Average", 5, 2),
            fz_gauss("Good", 10, 2)),
        fuzzy_alloc("Food", 2, 0, 10,
            fz_trapmf("Terrible", 0, 0, 1, 3),
            fz_trapmf("Delicious", 7, 9, 10, 10)),
        fuzzy_alloc("Tip", 3, 0, 30,
            fz_trimf("Low", 0, 5, 10),
            fz_trimf("Average", 10, 15, 20),
            fz_trimf("High", 20, 25, 30))
    };
    enum { fs_count = sizeof fs / sizeof fs[0] };

    Rule* rules[] = {
        rule_alloc(3,
            rule_lit(0, data_service, R_OR),
            rule_lit(0, data_food, R_EQUALS),
            rule_lit(0, data_tip, R_STOP)),
        rule_alloc(2,
            rule_lit(1, data_service, R_EQUALS),
            rule_lit(1, data_tip, R_STOP)),
        rule_alloc(3,
            rule_lit(2, data_service, R_OR),
            rule_lit(1, data_food, R_EQUALS),
            rule_lit(2, data_tip, R_STOP)),
    };
    enum { rules_count = sizeof rules / sizeof rules[0] };

    // Memberships
    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].count = fs[fs_count - 1]->count + 1;
        ms[i].items = malloc(sizeof(double) * (fs[fs_count - 1]->count + 1));
    }

    Array res[read_data];
    for (size_t i = 0; i < read_data; i++) {
        res[i].count = fs[data_tip]->count;
        res[i].items = malloc(sizeof(double) * fs[data_tip]->count);
    }

    double results[read_data] = { 0 };

    double total_diff_ape = 0;
    double total_diff_mae = 0;
    double total_diff_rmse = 0;

    for (size_t line = 0; line < read_data; line++) { // change 1 to  csv->data_count
        printf("forwarding for data %zu:\n", line + 1);

        printf("data %zu: ", line + 1);
        for (size_t i = 0; i < fs_count - 1; i++) {
            printf(" %s(%.2f), ", fs[i]->name, csv->datas[line][i]);
        }
        printf("\n");

        for (size_t class = 0; class < fs[fs_count - 1]->count; class++) {
            ms[class].count = fs[class]->count;
            fuzzy_forward(ms[class], fs[class], csv->datas[line][class]);

            printf("%s:\n\t", fs[class]->name);
            for (size_t j = 0; j < ms[class].count; j++) {
                printf("%.2f, ", ms[class].items[j]);
            }
            printf("\n");
        }

        rule_forward(res[line], fs, ms, rules, rules_count);

        double defuzz = fuzzy_defuzzify(fs[data_tip], res[line]);
        results[line] = defuzz;

        printf("res: ");
        for (size_t i = 0; i < res[line].count; i++) {
            printf("%.2f, ", res[line].items[i]);
        }
        printf("\n");

        double actual = csv->datas[line][data_tip];
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
}
