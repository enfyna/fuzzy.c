#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "fuzzy.h"
#include "graph.h"
#include "raymath.h"

#define LINE_SIZE 1024
#define DATA_COUNT 107

#define data_size 6
#define data_anchor_ratio 0
#define data_trans_range 1
#define data_node_density 2
#define data_iterations 3
#define data_ale 4
#define data_sd_ale 5

static const char* data_keys[data_size] = {
    "anchor_ratio",
    "trans_range",
    "node_density",
    "iterations",
    "ale",
    "sd_ale",
};

typedef struct {
    double data[data_size];
} data;

void data_print(data data)
{
    for (size_t i = 0; i < data_size; i++) {
        printf("%s: %lf\n", data_keys[i], data.data[i]);
    }
}

int main(void)
{
    fuzzy* fs[] = {
        fuzzy_alloc(3, 0.0, 8.0,
            fuzzy_func(func_x_mirror, -3.0),
            fuzzy_func(func_unit_trapezoid, 4.0),
            fuzzy_func(func_x, -5.0)),
        fuzzy_alloc(5, 0.0, 10.0,
            fuzzy_func(func_x_mirror, -2.0),
            fuzzy_func(func_unit_trapezoid, 3.0),
            fuzzy_func(func_unit_trapezoid, 4.0),
            fuzzy_func(func_unit_trapezoid, 5.0),
            fuzzy_func(func_x, -5.0)),
        fuzzy_alloc(3, 2.0, 10.0,
            fuzzy_func(func_x_mirror, -4.0),
            fuzzy_func(func_unit_trapezoid, 4.0),
            fuzzy_func(func_x, -6.0)),
        fuzzy_alloc(4, 0.0, 10.0,
            fuzzy_func(func_x_mirror, -3.0),
            fuzzy_func(func_unit_trapezoid, 3.5),
            fuzzy_func(func_unit_trapezoid, 6.0),
            fuzzy_func(func_x, -7.5)),
        fuzzy_alloc(5, 0.0, 10.0,
            fuzzy_func(func_x_mirror, -3.0),
            fuzzy_func(func_unit_trapezoid, 3.5),
            fuzzy_func(func_unit_trapezoid, 5.0),
            fuzzy_func(func_unit_trapezoid, 7.0),
            fuzzy_func(func_x, -7.5)),
    };
    int fs_count = sizeof fs / sizeof fs[0];

    double values[fs_count];
    memberships ms = {
        .values = values,
        .count = 3,
    };

    graph* g = get_graph(4, 800, 600, BLACK, DARKGRAY);
    line* line_x = get_line(g, 100, "X", MAROON);
    line* line_y = get_line(g, 100, "Y", VIOLET);

    for (int i = 0; i < 100; i++) {
        line_x->points[i] = (Vector2) { i * 2, 0 };
        line_y->points[i] = (Vector2) { 0, i * 2 };
    }

    line* lines[10];

    for (int i = 0; i < fs_count; i++) {
        int line_point_count = 1 + (fs[i]->bounds[1] - fs[i]->bounds[0]) * 10;

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
        assert(fs[i]->count <= title_count && "[ERROR] Add new title and color!");
        char buf[256] = { 0 };
        for (size_t j = 0; j < fs[i]->count; j++) {
            snprintf(buf, 256, title[j], data_keys[i]);
            lines[j] = get_line(g, line_point_count, strdup(buf), colors[j]);
        }

        int point_pos = 0;
        for (double j = fs[i]->bounds[0]; j < fs[i]->bounds[1]; j += 0.1) {
            ms.count = fs[i]->count;
            fuzzy_forward(ms, fs[i], j);
            // printf("%.1f: %.2f - %.2f - %.2f\n", j, ms.values[0], ms.values[1], ms.values[2]);
            for (size_t k = 0; k < ms.count; k++) {
                lines[k]->points[point_pos] = (Vector2) { j, ms.values[k] + (i * 2) };
            }
            point_pos++;
        }
        assert(line_point_count == point_pos);
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "mamdani");
    SetTargetFPS(144);

    Vector2 mouse_last_pressed;
    bool mouse_pressed = false;
    bool tab_pressed = false;
    (void)tab_pressed;

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
            g->scale = 1;
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

    FILE* f = fopen("res/mcs_ds_edited_iter_shuffled.csv", "r");
    assert(f != NULL && "[ERROR] Cant open file");

    data ds[DATA_COUNT];

    int i = 0;
    char buf[LINE_SIZE] = { 0 };
    char* line = fgets(buf, LINE_SIZE, f); // skip first line
    for (int i = 0; i < DATA_COUNT; i++) {
        line = fgets(buf, LINE_SIZE, f);
        assert(line != NULL && "[ERROR] DATA_COUNT is wrong!");

        // printf(">");
        char* token;
        const char* delim = ",\r\n";
        size_t idx = 0;
        for (token = strtok(line, delim); token != NULL; idx++, token = strtok(NULL, delim)) {
            // printf("%s,", token);
            ds[i].data[idx] = atof(token);
        }
        assert(idx == data_size && "[ERROR] Incomplete data!");
        // printf("\n");
    }

    data_print(ds[i]);

    return 0;
}
