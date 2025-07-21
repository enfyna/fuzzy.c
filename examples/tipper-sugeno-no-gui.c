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

#define data_i1 0
#define data_i2 1
#define data_result 2

int main(void)
{
    enum { read_data = 1 };
    Csv* csv = csv_alloc_read_file_until("res/sugeno.csv", read_data);
    assert(read_data == csv->data_count);

    Fuzzy* fs[] = {
        fuzzy_sugeno_alloc("I1", 3, 0, 15,
            fz_trapmf_sugeno("Low", 1, 0, 0, 5, 10),
            fz_trimf_sugeno("Medium", 2, 5, 10, 15),
            fz_trapmf_sugeno("High", 3, 10, 15, 100, 100)),
        fuzzy_sugeno_alloc("I2", 3, 0, 3,
            fz_trapmf_sugeno("Near", 1, 0, 0, 1, 2),
            fz_trimf_sugeno("Far", 2, 1, 2, 3),
            fz_trapmf_sugeno("Very Far", 3, 2, 3, 10, 10)),
    };
    enum { fs_count = sizeof fs / sizeof fs[0] };

    Rule* rules[] = {
        rule_alloc(FZ_SUGENO, 2,
            rule_lit(0, data_i1, R_AND),
            rule_lit(1, data_i2, R_STOP)),
        rule_alloc(FZ_SUGENO, 2,
            rule_lit(0, data_i1, R_AND),
            rule_lit(2, data_i2, R_STOP)),
        rule_alloc(FZ_SUGENO, 2,
            rule_lit(1, data_i1, R_AND),
            rule_lit(1, data_i2, R_STOP)),
        rule_alloc(FZ_SUGENO, 2,
            rule_lit(1, data_i1, R_AND),
            rule_lit(2, data_i2, R_STOP)),
    };
    enum { rules_count = sizeof rules / sizeof rules[0] };

    // Memberships
    double ms_items[fs_count][fs[fs_count - 1]->count + 1];
    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].count = fs[fs_count - 1]->count + 1;
        ms[i].items = ms_items[i];
    }

    double res_items[read_data][rules_count];
    Array res[read_data];
    for (size_t i = 0; i < read_data; i++) {
        res[i].count = rules_count;
        res[i].items = res_items[i];
    }

    double results[read_data] = { 0 };

    double total_diff_ape = 0;
    double total_diff_mae = 0;
    double total_diff_rmse = 0;

    for (size_t line = 0; line < read_data; line++) {
        printf("forwarding for data %zu:\n", line + 1);

        printf("data %zu: ", line + 1);
        for (size_t i = 0; i < fs_count; i++) {
            printf(" %s(%.2f), ", fs[i]->name, csv->datas[line][i]);
        }
        printf("\n");

        for (size_t class = 0; class < fs_count; class++) {
            ms[class].count = fs[class]->count;
            fuzzy_forward(ms[class], fs[class], csv->datas[line][class]);

            printf("%s:\n\t", fs[class]->name);
            for (size_t j = 0; j < ms[class].count; j++) {
                printf("%.2f, ", ms[class].items[j]);
            }
            printf("\n");
        }

        rule_forward(res[line], fs, ms, rules, rules_count);

        // calculate functional consequents
        double ys[rules_count];
        for (size_t j = 0; j < rules_count; j++) {
            printf("R%zu:\n  y%zu = ", j + 1, j + 1);
            Rule* r = rules[j];
            ys[j] = 0;
            for (size_t i = 0; i <= r->count; i++) {
                Fuzzy* f = fs[r->lits[i].idx_class];
                RuleLit rl = r->lits[i];
                printf("(%s(%.2f) * %.2lf)", f->name, f->mfs[rl.idx_cluster].scoef, csv->datas[line][i]);
                ys[j] += f->mfs[rl.idx_cluster].scoef * csv->datas[line][i];
                if (i < r->count) {
                    printf(" * ");
                } else {
                    printf(" = %.2lf\n", ys[j]);
                }
            }
        }

        logging("=============\n");

        // weighted average method
        double defuzz = 0;
        double ws = 0;
        for (size_t i = 0; i < rules_count; i++) {
            double w = res[line].items[i];
            double y = ys[i];
            defuzz += w * y;
            ws += w;
        }
        defuzz /= ws;

        results[line] = defuzz;

        printf("ws: ");
        for (size_t i = 0; i < res[line].count; i++) {
            printf("%.2f, ", res[line].items[i]);
        }
        printf("\n");

        double actual = csv->datas[line][data_result];
        double diff = actual - results[line];

        if (actual != 0) {
            total_diff_ape += fabs(diff) / (actual) * 100.0;
        }
        total_diff_mae += diff;
        total_diff_rmse += pow(diff, 2);

        printf("guess: %.2f | real: %.2f | diff: %.2f\n", results[line],
            actual, diff);
        printf("=============\n");
    }

    double mape = total_diff_ape / read_data;
    double mae = total_diff_mae / read_data;
    double rmse = sqrt(total_diff_rmse / read_data);
    printf("mae: %.2f | rmse: %.2f | mape: %.2f\n", mae, rmse, mape);
    printf("=============\n");

    for (size_t i = 0; i < fs_count; i++) {
        free(fs[i]);
    }

    for (size_t i = 0; i < rules_count; i++) {
        free(rules[i]);
    }

    csv_free(csv);

    return 0;
}
