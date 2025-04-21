#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

#include "raylib.h"

#define get_graph(...) get_graph_null(__VA_ARGS__, NULL)

#define X_AXIS 0
#define Y_AXIS 1

#define GRID_MARGIN 1.0

typedef struct {
    int pos_x;
    int pos_y;
    int width;
    int height;
    int section_count;
    char* names[5];
    double* values[5];
} bottom_pane;

typedef struct {
    Vector2* points;
    size_t count;
    Color color;
    const char* name;
} line;

typedef struct {
    line** items;
    size_t count;
    size_t capacity;
} lines;

typedef struct {
    double pos_x;
    double pos_y;
    double scale;
    double scale_x;
    double scale_y;
    double margin;
    double width;
    double height;
    double w_width;
    double w_height;
    lines lines;
    Color color;
    Color border_color;
    char* car_name;
    bottom_pane pane;
    Font font;
    int font_size;
} graph;

graph* get_graph_null(int margin, int width, int height, Color color, Color border_color, ...);
void graph_draw_axis(graph* g, int type, int offset, Color color);
void graph_draw_relative_line(graph* g, int type, int offset, Color color);
void graph_draw_lines(graph* g);
void graph_draw_line_value_at_x(graph* g, double pos);
void graph_draw_border(graph* g);
void graph_draw_bottom_pane(graph* g);
void graph_draw_grid(graph* g, Color color);
void graph_draw_point(graph* g, Vector2 p, int size, Color color);
void graph_zoom(graph* g, double zoom, double delta);
void graph_free(graph* g);

line* get_line(graph* g, int count, const char* name, Color color);
void create_h_line(Vector2* line, double* val, int count);
void create_v_line(Vector2* line, double* val, int count);
#endif // GRAPH_H
