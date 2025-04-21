#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "raylib.h"
#include "raymath.h"

static void _graph_draw_line(graph* g, line* l);

double min(double a, double b)
{
    return a < b ? a : b;
}

double max(double a, double b)
{
    return a > b ? a : b;
}

#define append(arr, l)                                                                         \
    {                                                                                          \
        do {                                                                                   \
            if ((arr)->count >= (arr)->capacity) {                                             \
                (arr)->capacity *= 2;                                                          \
                (arr)->items = realloc((arr)->items, sizeof(*(arr)->items) * (arr)->capacity); \
            }                                                                                  \
            (arr)->items[(arr)->count++] = (l);                                                \
        } while (0);                                                                           \
    }

line* get_line(graph* g, int count, const char* name, Color color)
{
    line* l = malloc(sizeof(line));
    l->name = name;
    l->color = color;
    l->count = count;
    l->points = malloc(sizeof(Vector2) * count);
    append(&g->lines, l);
    assert(l != NULL);
    assert(l->points != NULL);
    return l;
}

void create_h_line(Vector2* line, double* val, int count)
{
    for (int i = 0; i < count; i++) {
        line[i] = (Vector2) { i, val[i] };
    }
}

void create_v_line(Vector2* line, double* val, int count)
{
    for (int i = 0; i < count; i++) {
        line[i] = (Vector2) { i, val[i] };
    }
}

void graph_draw_axis(graph* g, int type, int offset, Color color)
{
    Vector2 points[2];
    line axis = {
        .color = color,
        .count = 2,
        .points = points,
    };
    double start, end;
    if (type == Y_AXIS) {
        start = -g->pos_x / (g->scale * g->scale_x);
        end = g->width / (g->scale * g->scale_x);
        axis.points[0] = (Vector2) { start + 1, offset };
        axis.points[1] = (Vector2) { start + end - 1, offset };
    } else {
        start = -g->pos_y / (g->scale * g->scale_y);
        end = g->height / (g->scale * g->scale_y);
        axis.points[0] = (Vector2) { offset, start + 1 };
        axis.points[1] = (Vector2) { offset, start + end - 1 };
    }
    _graph_draw_line(g, &axis);
}

void graph_draw_relative_line(graph* g, int type, int offset, Color color)
{
    int y = g->margin + g->height;
    if (type == X_AXIS) {
        DrawLine(
            g->margin, y - offset,
            g->margin + g->width, y - offset,
            color);
    } else if (type == Y_AXIS) {
        DrawLine(
            g->margin + offset, g->margin,
            g->margin + offset, g->margin + g->height,
            color);
    }
}

void graph_draw_lines(graph* g)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        line* l = g->lines.items[i];

        DrawRectangle(g->margin + 5, g->margin + 5 + 25 * i, 20, 20, l->color);
        DrawText(l->name, g->margin + 28, g->margin + 7 + 25 * i, 20, l->color);

        _graph_draw_line(g, l);
    }
}

static void _graph_draw_line(graph* g, line* line)
{
    Vector2 prev_point;

    bool continuous = true;

    for (size_t x = 0; x < line->count; x++) {
        Vector2 point = Vector2Scale(line->points[x], g->scale);
        point.x *= g->scale_x;
        point.y *= g->scale_y;

        point.x += g->margin + g->pos_x;
        point.y = g->margin + g->height - point.y - g->pos_y;

        if (point.x < g->margin) {
            continuous = false;
            continue;
        } else if (point.x > g->width + g->margin) {
            continuous = false;
            continue;
        }

        if (point.y < g->margin) {
            continuous = false;
            continue;
        } else if (point.y > g->height + g->margin) {
            continuous = false;
            continue;
        }

        if (continuous && x > 0) {
            DrawLineEx(
                point,
                prev_point,
                2, line->color);
        } else {
            continuous = true;
        }
        prev_point = point;
    }
}

void graph_draw_line_value_at_x(graph* g, double x)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        line* line = g->lines.items[i];
        for (size_t j = 0; j < line->count; j++) {
            Vector2 point = line->points[j];
            if (fabs(x - point.x) <= 0.1) {
                graph_draw_point(g, point, 4, line->color);
                break;
            };
        }
    }
}

void graph_draw_grid(graph* g, Color color)
{
    double margin = GRID_MARGIN * g->scale;
    for (double x = fmod(g->pos_y, margin * g->scale_y); x < g->height; x += margin * g->scale_y) {
        graph_draw_relative_line(g, 0, x, color);
    }
    for (double y = fmod(g->pos_x, margin * g->scale_x); y < g->width; y += margin * g->scale_x) {
        graph_draw_relative_line(g, 1, y, color);
    }
}

void graph_draw_bottom_pane(graph* g)
{
    DrawText(
        TextFormat("Selected Car: %s", g->car_name),
        g->margin + 5, g->margin + g->height - 25,
        18, WHITE);

    DrawText(
        TextFormat("%%%.2lf", g->scale),
        g->margin + g->width - 75, g->margin + g->height - 25,
        24, WHITE);

    if (g->pane.section_count <= 0 || g->pane.height == -1) {
        return;
    }
    int section_width = g->pane.width / g->pane.section_count;
    for (int i = 0; i < g->pane.section_count; i++) {
        int pos_x = g->pane.pos_x + section_width * i;
        int pos_y = g->pane.pos_y;
        const char* val_str = TextFormat("%lf", *g->pane.values[i]);
        Vector2 size_name = MeasureTextEx(
            g->font, g->pane.names[i], g->font_size, 0);
        Vector2 size_value = MeasureTextEx(
            g->font, val_str, g->font_size, 0);
        float padding = (g->pane.height - (size_name.y + size_value.y)) / 2;
        if (padding <= 2) {
            g->font_size -= 0.1;
            return;
        }
        DrawText(g->pane.names[i],
            pos_x + padding,
            pos_y + padding,
            g->font_size, WHITE);
        DrawText(val_str,
            pos_x + padding,
            pos_y + padding + size_value.y,
            g->font_size, WHITE);
        DrawLineV(
            (Vector2) { pos_x, pos_y },
            (Vector2) { pos_x, pos_y + g->pane.height },
            g->border_color);
    }
}

void graph_draw_border(graph* g)
{
    // graph
    DrawRectangle(
        g->margin, g->margin,
        g->width, g->height,
        g->color);
    // right
    DrawLine(
        g->margin, g->margin,
        g->margin, g->height + g->margin,
        g->border_color);
    // bottom
    DrawLine(
        g->margin, g->height + g->margin,
        g->width + g->margin, g->height + g->margin,
        g->border_color);
    // top
    DrawLine(
        g->margin, g->margin,
        g->width + g->margin, g->margin,
        g->border_color);
    // left
    DrawLine(
        g->width + g->margin, g->margin,
        g->width + g->margin, g->height + g->margin,
        g->border_color);

    if (g->pane.height == -1) {
        return;
    }
    // bottom pane
    DrawRectangle(
        g->pane.pos_x, g->pane.pos_y,
        g->width, g->pane.height,
        g->color);
    // bottom
    DrawLine(
        g->pane.pos_x, g->pane.pos_y + g->pane.height,
        g->pane.pos_x + g->pane.width, g->pane.pos_y + g->pane.height,
        g->border_color);
    // top
    DrawLine(
        g->pane.pos_x, g->pane.pos_y,
        g->pane.pos_x + g->pane.width, g->pane.pos_y,
        g->border_color);
    // right
    DrawLine(
        g->pane.pos_x + g->pane.width, g->pane.pos_y,
        g->pane.pos_x + g->pane.width, g->pane.pos_y + g->pane.height,
        g->border_color);
    // left
    DrawLine(
        g->pane.pos_x, g->pane.pos_y,
        g->pane.pos_x, g->pane.pos_y + g->pane.height,
        g->border_color);
}

void graph_draw_point(graph* g, Vector2 point, int size, Color color)
{
    Vector2 real_coordinates = point;
    point = Vector2Scale(point, g->scale);
    point.x *= g->scale_x;
    point.y *= g->scale_y;

    point.x += g->margin + g->pos_x;
    point.y = g->margin + g->height - point.y - g->pos_y;

    if (point.x < g->margin) {
        return;
    } else if (point.x > g->width + g->margin) {
        return;
    }
    if (point.y < g->margin) {
        return;
    } else if (point.y > g->height + g->margin) {
        return;
    }
    float selected_point;
    if (real_coordinates.y != 0) {
        selected_point = real_coordinates.y;
    } else {
        selected_point = real_coordinates.x;
    }
    DrawText(TextFormat("%.1lf", selected_point), point.x, point.y + 4, 20, color);
    DrawCircleV(point, size, color);
}

void graph_zoom(graph* g, double zoom, double delta)
{
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        g->scale_x += zoom * delta;
    } else if (IsKeyDown(KEY_LEFT_ALT)) {
        g->scale_y += zoom * delta;
    } else {
        g->scale += zoom * delta;
    }
    if (g->scale_y <= 0.01) {
        g->scale_y = 0.01;
    }
    if (g->scale_x <= 0.01) {
        g->scale_x = 0.01;
    }
    if (g->scale <= 0.01) {
        g->scale = 0.01;
    }
}

void graph_free(graph* g)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        free(g->lines.items[i]->points);
        free(g->lines.items[i]);
    }
    free(g->lines.items);
    free(g);
}

graph* get_graph_null(int margin, int width, int height, Color color, Color border_color, ...)
{
    graph* g = malloc(sizeof(graph));
    assert(g != NULL);

    g->font = GetFontDefault();
    g->font_size = 24;

    va_list ap;
    va_start(ap, border_color);

    char* title;
    double* value;

    g->pane.section_count = 0;

    while ((title = va_arg(ap, char*)) && (value = va_arg(ap, double*))) {
        g->pane.names[g->pane.section_count] = title;
        g->pane.values[g->pane.section_count] = value;

        g->pane.section_count++;
    }
    va_end(ap);

    g->pos_x = 0;
    g->pos_y = 0;
    g->scale = 1.0;
    g->scale_x = 1.0;
    g->scale_y = 1.0;

    g->border_color = border_color;
    g->color = color;

    g->margin = margin;

    g->w_width = width;
    g->w_height = height;

    if (g->pane.section_count > 0) {
        g->height = height - g->margin * 6;
        g->width = width - g->margin * 2;

        g->pane.pos_x = g->margin;
        g->pane.pos_y = g->height + g->margin * 2;
        g->pane.width = g->width;
        g->pane.height = g->margin * 3;
    } else {
        g->height = height - g->margin * 2;
        g->width = width - g->margin * 2;

        g->pane.pos_x = -1;
        g->pane.pos_y = -1;
        g->pane.width = -1;
        g->pane.height = -1;
    }

    g->lines.count = 0;
    g->lines.capacity = 4;
    g->lines.items = malloc(sizeof(line*) * g->lines.capacity);
    assert(g->lines.items != NULL);

    return g;
}
