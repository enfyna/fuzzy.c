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
#include "raymath.h"

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
Color colors[] = {
    LIME,
    GREEN,
    ORANGE,
    RED,
    PURPLE,
};
size_t title_count = sizeof title / sizeof title[0];
size_t color_count = sizeof colors / sizeof colors[0];
size_t class_count = sizeof title / sizeof title[0];

int main(void)
{
    assert(title_count == color_count);

    const int read_data = 100;

    Csv* c = csv_alloc_read_file_until("res/mcs_ds_edited_iter_shuffled.csv",
        read_data);

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
            rule_lit(R_MED, data_node_density, R_AND),
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
            rule_lit(R_LOW, data_node_density, R_AND),
            rule_lit(R_HIGH, data_iterations, R_EQUALS),
            rule_lit(R_LOW, data_ale, R_STOP)),
    };
    size_t rules_count = sizeof rules / sizeof rules[0];

    double ale_center_points[] = { 0, 1, 2 };

    Fuzzy* fs[] = {
        // data_anchor_ratio
        fuzzy_alloc(5, 10.0, 30.0,
            fuzzy_mf(mf_x_mirror, 17),
            fuzzy_mf(mf_unit_triangle, 18.0),
            fuzzy_mf(mf_unit_triangle, 20.0),
            fuzzy_mf(mf_unit_triangle, 22.0),
            fuzzy_mf(mf_x, 23.0)),
        // data_trans_range
        fuzzy_alloc(5, 15.0, 25.0,
            fuzzy_mf(mf_x_mirror, -17),
            fuzzy_mf(mf_unit_triangle, 18.0),
            fuzzy_mf(mf_unit_triangle, 20.0),
            fuzzy_mf(mf_unit_triangle, 22.0),
            fuzzy_mf(mf_x, -23)),
        // data_node_density
        fuzzy_alloc(5, 100.0, 300.0,
            fuzzy_mf(mf_unit_triangle, 100.0),
            fuzzy_mf(mf_unit_triangle, 99.5),
            fuzzy_mf(mf_unit_triangle, 200.0),
            fuzzy_mf(mf_unit_triangle, 299.5),
            fuzzy_mf(mf_unit_triangle, 300.0)),
        // data_iterations
        fuzzy_alloc(5, 15.0, 100.0,
            fuzzy_mf(mf_x_mirror, -49),
            fuzzy_mf(mf_unit_triangle, 50),
            fuzzy_mf(mf_unit_triangle, 52),
            fuzzy_mf(mf_unit_triangle, 54),
            fuzzy_mf(mf_x, -55)),
        // data_ale
        fuzzy_alloc(3, 0.0, 3.0,
            fuzzy_mf(mf_unit_triangle, ale_center_points[0]),
            fuzzy_mf(mf_unit_triangle, ale_center_points[1]),
            fuzzy_mf(mf_unit_triangle, ale_center_points[2])),
    };
    size_t fs_count = sizeof fs / sizeof fs[0];

    graph* g = get_graph(0, 800, 600, BLACK, DARKGRAY);

    for (size_t i = 0; i < fs_count; i++) {
        assert(fs[i]->count <= title_count
            && "[ERROR] Add new title and color!");

        line* buf_line[fs[i]->count];

        int line_point_count = 1 + (fs[i]->bounds[1] - fs[i]->bounds[0]) * 10;

        char buf[256] = { 0 };
        for (size_t j = 0; j < fs[i]->count; j++) {
            snprintf(buf, 256, title[j], c->titles[i]);
            buf_line[j]
                = get_line(g, line_point_count, strdup(buf), colors[j]);
        }

        double values[fs_count];
        Array m = { .items = values };

        double point_pos = fs[i]->bounds[0];
        for (int j = 0; j < line_point_count; j++) {
            m.count = fs[i]->count;
            fuzzy_forward(m, fs[i], point_pos);
            for (size_t k = 0; k < m.count; k++) {
                buf_line[k]->points[j] = (Vector2) {
                    norm(fs[i]->bounds[0], fs[i]->bounds[1], point_pos),
                    m.items[k] + (i * 3)
                };
            }
            point_pos += 0.1;
        }
    }

    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].items = malloc(sizeof(double) * (class_count + 1));
    }

    Array res = {
        .count = fs[data_ale]->count,
        .items = malloc(sizeof(double) * fs[data_ale]->count),
    };

    double total_diff_mae = 0;
    double total_diff_rmse = 0;

    for (size_t j = 0; j < c->data_count; j++) {
        printf("%zu:\n", j);
        for (size_t i = 0; i < class_count - 1; i++) {
            ms[i].count = fs[i]->count;
            fuzzy_forward(ms[i], fs[i], c->datas[j][i]);

            printf("%s:\n\t", c->titles[i]);
            for (size_t j = 0; j < ms[i].count; j++) {
                printf("%.2f, ", ms[i].items[j]);
            }
            printf("\n");
        }

        rule_forward(res, ms, rules, rules_count);

        double ale = 0;
        double res_sum = 0;

        printf("res: ");
        for (size_t i = 0; i < res.count; i++) {
            ale += res.items[i] * ale_center_points[i];
            res_sum += res.items[i];
            printf("%.2f, ", res.items[i]);
        }
        if (res_sum == 0) {
            ale = 0;
        } else {
            ale /= res_sum;
        }
        printf("\n");
        double diff = c->datas[j][data_ale] - ale;
        total_diff_mae += diff;
        total_diff_rmse += pow(diff, 2);
        printf("guess ale: %f | real ale: %f | diff: %f\n", ale,
            c->datas[j][data_ale], diff);
        printf("=============\n");
    }
    // TODO: mape
    double mae = total_diff_mae / read_data;
    double rmse = sqrt(total_diff_rmse / read_data);
    printf("mae: %f | rmse: %f\n", mae, rmse);
    printf("=============\n");

    // exit(1);

    line* line_x = get_line(g, 100, "X", MAROON);
    line* line_y = get_line(g, 100, "Y", VIOLET);
    for (int i = 0; i < 100; i++) {
        line_x->points[i] = (Vector2) { i, 0 };
        line_y->points[i] = (Vector2) { 0, i };
    }

    line* trimff = get_line(g, 1000, "TRIMF", RED);
    line* gauss = get_line(g, 1000, "GAUSS", RED);

    MF2 mf2_trimf = (MF2) {
        .mf = mf_trimf,
        .args_count = 3,
        .args = { 28, 30, 32 }
    };

    MF2 mf2_gauss = (MF2) {
        .mf = mf_gauss,
        .args_count = 2,
        .args = { 30, 0.5 }
    };

    for (int i = 0; i < 1000; i++) {
        double x = i / 10.0;
        double val = mf_forward_v2(mf2_trimf, x);
        trimff->points[i] = (Vector2) { x, val };
        val = mf_forward_v2(mf2_gauss, x);
        gauss->points[i] = (Vector2) { x, val };
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "mamdani");
    SetTargetFPS(144);

    Vector2 mouse_last_pressed;
    bool mouse_pressed = false;

    g->scale_x = 1;
    g->scale_y = 1;
    g->scale = 30.0;
    g->pos_y = 0;
    g->pos_x = GetScreenWidth() / 2.0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        g->width = GetScreenWidth() - g->margin * 2;
        g->height = GetScreenHeight() - g->margin * 2;

        double delta = GetFrameTime();

        double zoom_speed = GetMouseWheelMove();
        if (zoom_speed != 0) {
            zoom_speed *= IsKeyDown(KEY_LEFT_SHIFT) ? 30.0 * g->scale : 20.0;
            graph_zoom(g, zoom_speed, delta);
        }

        if (!mouse_pressed && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            mouse_last_pressed = GetMousePosition();
            mouse_pressed = true;
        } else if (mouse_pressed) {
            double dx = GetMouseX() - mouse_last_pressed.x;
            double dy = GetMouseY() - mouse_last_pressed.y;

            g->pos_y -= dy * 2.5;
            g->pos_x += dx * 2.5;

            mouse_pressed = false;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            g->scale_x = 1;
            g->scale_y = 1;
            g->scale = 30.0;
            g->pos_y = 0;
            g->pos_x = GetScreenWidth() / 2.0;
        }

        graph_draw_border(g);
        graph_draw_grid(g, DARKGRAY);

        graph_draw_lines(g);

        if (IsKeyDown(KEY_TAB)) {
            Vector2 m_pos = GetMousePosition();
            // normalize mouse position to graph
            m_pos.x -= g->margin + g->pos_x;
            m_pos.y = g->margin + g->height - m_pos.y - g->pos_y;
            m_pos = Vector2Scale(m_pos, 1.0 / g->scale);
            m_pos.x *= 1.0 / g->scale_x;
            m_pos.y *= 1.0 / g->scale_y;
            graph_draw_line_value_at_x(g, m_pos.x);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
