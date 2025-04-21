#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "data.h"
#include "fuzzy.h"
#include "graph.h"
#include "raymath.h"

Rule rules[20] = {
    // (Rule) { .op = R_AND, .data = { data_anchor_ratio, R_HIGH, data_trans_range, R_LOW, data_ale, R_LOW } },
    // (Rule) { .op = R_AND, .data = { data_anchor_ratio, R_HIGH, data_trans_range, R_LOW, data_ale, R_LOW } },
    // (Rule) { .op = R_AND, .data = { data_anchor_ratio, R_HIGH, data_trans_range, R_LOW, data_ale, R_LOW } },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
};

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

    Csv* c = csv_alloc_read_file_until("res/mcs_ds_edited_iter_shuffled.csv", 10);
    // csv_print(c);

    fuzzy* fs[] = {
        fuzzy_alloc(3, 10.0, 30.0,
            fuzzy_func(func_x_mirror, -15.5),
            fuzzy_func(func_unit_trapezoid, 16.0),
            fuzzy_func(func_x, -17.5)),
        fuzzy_alloc(5, 15.0, 25.0,
            fuzzy_func(func_x_mirror, -17.5),
            fuzzy_func(func_unit_gaussian, 18),
            fuzzy_func(func_parabolica, 20),
            fuzzy_func(func_unit_trapezoid, 22),
            fuzzy_func(func_x, -23)),
        fuzzy_alloc(2, 100.0, 300.0,
            fuzzy_func(func_x_mirror, -201.0),
            fuzzy_func(func_x, -200.0)),
        fuzzy_alloc(5, 15.0, 100.0,
            fuzzy_func(func_x_mirror, -46),
            fuzzy_func(func_trapezoid_5, 46),
            fuzzy_func(func_trapezoid_5, 52),
            fuzzy_func(func_trapezoid_5, 58),
            fuzzy_func(func_x, -63)),
        fuzzy_alloc(3, 0.0, 1.0,
            fuzzy_func(func_unit_triangle, -0.5),
            fuzzy_func(func_unit_triangle, 0.5),
            fuzzy_func(func_unit_triangle, 1.5)),
    };
    size_t fs_count = sizeof fs / sizeof fs[0];

    double values[fs_count];
    memberships ms = {
        .values = values,
        .count = 3,
    };

    graph* g = get_graph(4, 800, 600, BLACK, DARKGRAY);
    line* line_x = get_line(g, 100, "X", MAROON);
    line* line_y = get_line(g, 100, "Y", VIOLET);

    for (int i = 0; i < 100; i++) {
        line_x->points[i] = (Vector2) { i, 0 };
        line_y->points[i] = (Vector2) { 0, i };
    }

#define buf_line_count 10
    line* buf_line[buf_line_count];

    for (size_t i = 0; i < fs_count; i++) {
        int line_point_count = 1 + (fs[i]->bounds[1] - fs[i]->bounds[0]) * 10;
        assert(fs[i]->count <= title_count && "[ERROR] Add new title and color!");
        assert(fs[i]->count < buf_line_count && "[ERROR] Increase buf_line_count !");
        char buf[256] = { 0 };
        for (size_t j = 0; j < fs[i]->count; j++) {
            snprintf(buf, 256, title[j], c->titles[i]);
            buf_line[j] = get_line(g, line_point_count, strdup(buf), colors[j]);
        }

        double point_pos = fs[i]->bounds[0];
        for (int j = 0; j < line_point_count; j++) {
            // printf("%d -- %lf\n", j, fs[i]->bounds[1]);
            ms.count = fs[i]->count;
            fuzzy_forward(ms, fs[i], point_pos);
            // printf("%.1f: %.2f - %.2f - %.2f\n", j, ms.values[0], ms.values[1], ms.values[2]);
            for (size_t k = 0; k < ms.count; k++) {
                buf_line[k]->points[j] = (Vector2) { point_pos, ms.values[k] + (i * 2) };
            }
            point_pos += 0.1;
        }
    }

    memberships mss[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        mss[i].values = malloc(sizeof(double) * class_count);
    }

    for (size_t j = 0; j < c->data_count; j++) {
        printf("%zu:\n", j);
        for (size_t i = 0; i < class_count - 1; i++) {
            mss[i].count = fs[i]->count;
            fuzzy_forward(mss[i], fs[i], c->datas[j][i]);

            printf("\t%s:\n\t", c->titles[i]);
            for (size_t j = 0; j < mss[i].count; j++) {
                printf("%.2f, ", mss[i].values[j]);
            }
            printf("\n");
        }
    }

    // exit(123);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "mamdani");
    SetTargetFPS(144);

    Vector2 mouse_last_pressed;
    bool mouse_pressed = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        g->width = GetScreenWidth() - g->margin * 2;
        g->height = GetScreenHeight() - g->margin * 2;

        double delta = GetFrameTime();

        double zoom_speed = GetMouseWheelMove();
        if (zoom_speed != 0) {
            zoom_speed *= IsKeyDown(KEY_LEFT_SHIFT) ? 25.0 * g->scale : 5.0;
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
            g->scale = 10.0;
            g->pos_y = 0;
            g->pos_x = 0;
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
