#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "csv.h"
#include "fuzzy.h"
#include "graph.h"

#define data_anchor_ratio 0
#define data_trans_range 1
#define data_node_density 2
#define data_iterations 3
#define data_ale 4
#define data_sd_ale 5

Color colors[] = {
    LIME,
    GREEN,
    ORANGE,
    RED,
    PURPLE,
};
size_t color_count = sizeof colors / sizeof colors[0];

int main(void)
{
    enum { read_data = 100 };
    Csv* csv = csv_alloc_read_file_until("res/mcs_ds_edited_iter_shuffled.csv",
        read_data);
    assert(read_data == csv->data_count);

    Fuzzy* fs[] = {
        fuzzy_alloc("Anchor Ratio", 5, 10.0, 30.0,
            fz_trimf("very low", 10, 10, 25),
            fz_trimf("low", 10, 20, 30),
            fz_trimf("med", 10, 20, 30),
            fz_trimf("high", 10, 20, 30),
            fz_trimf("very high", 25, 30, 31)),
        fuzzy_alloc("Trans Range", 5, 15.0, 25.0,
            fz_gauss("very low", 15, 1),
            fz_gauss("low", 18, 1),
            fz_gauss("med", 20, 1),
            fz_gauss("high", 22, 1),
            fz_gauss("very high", 25, 1)),
        fuzzy_alloc("Node Density", 5, 100.0, 300.0,
            fz_gauss("very low", 100, 25),
            fz_gauss("low", 150, 25),
            fz_gauss("med", 200, 25),
            fz_gauss("high", 250, 25),
            fz_gauss("very high", 300, 25)),
        fuzzy_alloc("Iterations", 5, 15.0, 100.0,
            fz_gauss("very low", 15, 10),
            fz_gauss("low", 30, 10),
            fz_gauss("med", 50, 10),
            fz_gauss("high", 70, 10),
            fz_gauss("very high", 100, 10)),
        fuzzy_alloc("ALE", 3, 0.0, 3.0,
            fz_gauss("low", 0, 0.5),
            fz_gauss("med", 1, 0.5),
            fz_gauss("high", 2, 0.5))
    };
    enum { fs_count = sizeof fs / sizeof fs[0] };

    Rule* rules[] = {
        rule_alloc(FZ_MAMDANI, 2,
            rule_lit(1, data_anchor_ratio, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 2,
            rule_lit(3, data_node_density, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 2,
            rule_lit(0, data_trans_range, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 2,
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 2,
            rule_lit(1, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_anchor_ratio, R_AND),
            rule_lit(0, data_trans_range, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_anchor_ratio, R_AND),
            rule_lit(1, data_trans_range, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_node_density, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(1, data_node_density, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_anchor_ratio, R_AND),
            rule_lit(3, data_trans_range, R_EQUALS),
            rule_lit(0, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(3, data_trans_range, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(2, data_trans_range, R_AND),
            rule_lit(3, data_node_density, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(3, data_anchor_ratio, R_AND),
            rule_lit(1, data_node_density, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(2, data_anchor_ratio, R_AND),
            rule_lit(2, data_trans_range, R_AND),
            rule_lit(2, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(2, data_anchor_ratio, R_AND),
            rule_lit(2, data_node_density, R_AND),
            rule_lit(2, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 4,
            rule_lit(2, data_trans_range, R_AND),
            rule_lit(2, data_node_density, R_AND),
            rule_lit(2, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 5,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(3, data_trans_range, R_AND),
            rule_lit(3, data_node_density, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 5,
            rule_lit(3, data_anchor_ratio, R_AND),
            rule_lit(1, data_trans_range, R_AND),
            rule_lit(1, data_node_density, R_AND),
            rule_lit(1, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 5,
            rule_lit(2, data_anchor_ratio, R_AND),
            rule_lit(2, data_trans_range, R_AND),
            rule_lit(2, data_node_density, R_OR),
            rule_lit(2, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 5,
            rule_lit(1, data_anchor_ratio, R_AND),
            rule_lit(3, data_trans_range, R_AND),
            rule_lit(1, data_node_density, R_AND),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(2, data_ale, R_STOP)),
        rule_alloc(FZ_MAMDANI, 5,
            rule_lit(3, data_anchor_ratio, R_AND),
            rule_lit(3, data_trans_range, R_AND),
            rule_lit(1, data_node_density, R_OR),
            rule_lit(3, data_iterations, R_EQUALS),
            rule_lit(1, data_ale, R_STOP)),
    };
    enum { rules_count = sizeof rules / sizeof rules[0] };

    // Memberships
    double ms_items[fs_count][fs[fs_count - 1]->count + 1];
    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].count = fs[fs_count - 1]->count + 1;
        ms[i].items = ms_items[i];
    }

    double res_items[read_data][fs[data_ale]->count];
    Array res[read_data];
    for (size_t i = 0; i < read_data; i++) {
        res[i].count = fs[data_ale]->count;
        res[i].items = res_items[i];
    }

    double results[read_data] = { 0 };

    double total_diff_ape = 0;
    double total_diff_mae = 0;
    double total_diff_rmse = 0;

    for (size_t line = 0; line < read_data; line++) { // change 1 to  csv->data_count
        printf("forwarding for data %zu:\n", line + 1);

        printf("data %zu: ", line + 1);
        for (size_t i = 0; i < fs_count - 1; i++) {
            printf(" %s(%.2f), ", csv->titles[i], csv->datas[line][i]);
        }
        printf("\n");

        for (size_t class = 0; class < fs[fs_count - 1]->count - 1; class++) {
            ms[class].count = fs[class]->count;
            fuzzy_forward(ms[class], fs[class], csv->datas[line][class]);

            printf("%s:\n\t", csv->titles[class]);
            for (size_t j = 0; j < ms[class].count; j++) {
                printf("%.2f, ", ms[class].items[j]);
            }
            printf("\n");
        }

        rule_forward(res[line], fs, ms, rules, rules_count);

        double defuzz = defuzz_centroid(fs[data_ale], res[line]);
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

        printf("guess: %.2f | real: %.2f | diff: %.2f\n", results[line],
            actual, diff);
        printf("=============\n");
    }

    double mape = total_diff_ape / read_data;
    double mae = total_diff_mae / read_data;
    double rmse = sqrt(total_diff_rmse / read_data);
    printf("mae: %.2f | rmse: %.2f | mape: %.2f\n", mae, rmse, mape);
    printf("=============\n");

    SetTargetFPS(144);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1900, 1000, "Network");

    Graph g = graph(1, RED, BLACK, DARKGRAY);
    g.show_legend = false;

    Graph gm = graph(1, GREEN, BLACK, DARKGRAY);
    gm.show_legend = false;

    for (size_t i = 0; i < fs_count; i++) {
        Line* buf_line[fs[i]->count];

        int line_point_count = 101;

        for (size_t j = 0; j < fs[i]->count; j++) {
            buf_line[j] = line_alloc(&g, line_point_count, fs[i]->mfs[j].name, colors[j], 2);
        }

        double values[fs_count];
        Array m = { .items = values };

        double point_pos = 0;
        for (int j = 0; j < line_point_count; j++) {
            m.count = fs[i]->count;
            fuzzy_forward(m, fs[i], fz_lerp(fs[i], point_pos));
            for (size_t k = 0; k < m.count; k++) {
                buf_line[k]->points[j] = (Vector2) {
                    point_pos, m.items[k] + (i * 2)
                };
            }
            point_pos += 0.01;
        }
    }

    for (size_t line = 0; line < read_data; line++) {
        const size_t line_point_count = 101;
        Line* r = line_alloc(&gm, line_point_count, "Defuzz", RED, 2);
        Line* a = line_alloc(&gm, line_point_count, "Actual", GREEN, 2);
        for (size_t i = 0; i < fs[data_ale]->count; i++) {
            Line* l = line_alloc(&gm, line_point_count, fs[data_ale]->mfs[i].name, colors[i], 2);

            double point_pos = 0;
            fs[data_ale]->mfs[i].weight = res[line].items[i];
            for (size_t j = 0; j < line_point_count; j++) {
                double p = mf_forward(fs[data_ale]->mfs[i], fz_lerp(fs[data_ale], point_pos));
                l->points[j] = (Vector2) { .x = point_pos + line, .y = p };
                double res = (results[line] - fz_min(fs[data_ale]) / (fz_max(fs[data_ale]) - fz_min(fs[data_ale])));
                double actual = (csv->datas[line][data_ale] - fz_min(fs[data_ale])) / (fz_max(fs[data_ale]) - fz_min(fs[data_ale]));
                r->points[j] = (Vector2) { .x = res + line, .y = 2 * point_pos - 0.2 };
                a->points[j] = (Vector2) { .x = actual + line, .y = 2 * point_pos - 0.2 };
                point_pos += 0.01;
            }
        }
    }

    Line* line_x = line_alloc(&g, 100, "X", MAROON, 2);
    Line* line_y = line_alloc(&g, 100, "Y", VIOLET, 2);
    for (int i = 0; i < 100; i++) {
        line_x->points[i] = (Vector2) { i, 0 };
        line_y->points[i] = (Vector2) { 0, i };
    }
    line_x = line_alloc(&gm, 100, "X", MAROON, 2);
    line_y = line_alloc(&gm, 100, "Y", VIOLET, 2);
    for (int i = 0; i < 100; i++) {
        line_x->points[i] = (Vector2) { i, 0 };
        line_y->points[i] = (Vector2) { 0, i };
    }

    g.st_reset.pos = (Vector2) { 430, 270 };
    g.st_reset.scale = (Vector3) { 1, 1, 90 };
    g.st_current = g.st_reset;

    gm.st_reset.pos = (Vector2) { 205, 320 };
    gm.st_reset.scale = (Vector3) { 1, 1, 112 };
    gm.st_current = gm.st_reset;

    Graph* glist[] = { &g, &gm };
    size_t glist_count = sizeof glist / sizeof glist[0];

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        for (size_t i = 0; i < glist_count; i++) {
            Graph* gp = glist[i];

            if (gp == &g) {
                gp->bound = (Rectangle) {
                    10, 10, (GetScreenWidth() / 2.0) - 20, GetScreenHeight() - 20
                };
            } else if (gp == &gm) {
                gp->bound = (Rectangle) {
                    GetScreenWidth() / 2.0 + 10, 10, GetScreenWidth() / 2.0 - 20, GetScreenHeight() - 20
                };
            }

            graph_update(gp);

            graph_draw(gp);
        }

        EndDrawing();
    }

    graph_free(&g);
    graph_free(&gm);

    CloseWindow();

    for (size_t i = 0; i < fs_count; i++) {
        free(fs[i]);
    }

    for (size_t i = 0; i < rules_count; i++) {
        free(rules[i]);
    }

    csv_free(csv);

    return 0;
}
