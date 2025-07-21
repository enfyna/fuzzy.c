#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fuzzy.h"
#include "graph.h"
#include "raylib.h"
#include "raymath.h"

#define data_distance 0
#define data_angle 1
#define data_deviation 2

#define vec2_rand(min, max) \
    (Vector2) { .x = rand() % (max - min) + min, .y = rand() % (max - min) + min }

typedef struct {
    size_t size;
    Vector2 pos;
    Vector2 speed;
} Robot;

double angle_between_points(Vector2 right, Vector2 middle, Vector2 left)
{
    // Vector from middle to left
    double v1x = left.x - middle.x;
    double v1y = left.y - middle.y;

    // Vector from middle to right
    double v2x = right.x - middle.x;
    double v2y = right.y - middle.y;

    // Normalize both vectors
    double mag1 = sqrt(v1x * v1x + v1y * v1y);
    double mag2 = sqrt(v2x * v2x + v2y * v2y);

    if (mag1 == 0 || mag2 == 0)
        return 0.0; // undefined angle

    v1x /= mag1;
    v1y /= mag1;
    v2x /= mag2;
    v2y /= mag2;

    // Compute angle using atan2 of cross and dot product
    double cross = v1x * v2y - v1y * v2x;
    double dot = v1x * v2x + v1y * v2y;

    double angle_rad = atan2(cross, dot); // signed angle in radians
    double angle_deg = angle_rad * (180.0 / M_PI);

    return angle_deg;
}

int main(void)
{

    Fuzzy* cs[] = {
        fuzzy_alloc("distance", 4, 0, 2.2,
            fz_trimf("very near", 0, 0, 0.8),
            fz_trimf("near", 0, 0.8, 1.5),
            fz_trimf("far", 0.8, 1.5, 2.2),
            fz_trapmf("very far", 1.5, 2.2, 100, 101)),
        fuzzy_alloc("angle", 5, -90, 90,
            fz_trapmf("left", -100, -100, -90, -45),
            fz_trimf("ahead left", -90, -45, 0),
            fz_trimf("ahead", -45, 0, 45),
            fz_trimf("ahead right", 0, 45, 90),
            fz_trapmf("right", 45, 90, 100, 100)),
        fuzzy_alloc("deviation", 5, -90, 90,
            fz_trapmf("left", -100, -100, -90, -45),
            fz_trimf("ahead left", -90, -45, 0),
            fz_trimf("ahead", -45, 0, 45),
            fz_trimf("ahead right", 0, 45, 90),
            fz_trapmf("right", 45, 90, 100, 100)),
    };
    enum { fs_count = sizeof cs / sizeof cs[0] };

    Rule* rules[] = {
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_distance, R_AND),
            rule_lit(0, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_distance, R_AND),
            rule_lit(1, data_angle, R_EQUALS),
            rule_lit(3, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_distance, R_AND),
            rule_lit(2, data_angle, R_EQUALS),
            rule_lit(1, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_distance, R_AND),
            rule_lit(3, data_angle, R_EQUALS),
            rule_lit(1, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(0, data_distance, R_AND),
            rule_lit(4, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),

        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_distance, R_AND),
            rule_lit(0, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_distance, R_AND),
            rule_lit(1, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_distance, R_AND),
            rule_lit(2, data_angle, R_EQUALS),
            rule_lit(4, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_distance, R_AND),
            rule_lit(3, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(1, data_distance, R_AND),
            rule_lit(4, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),

        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_distance, R_AND),
            rule_lit(0, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_distance, R_AND),
            rule_lit(1, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_distance, R_AND),
            rule_lit(2, data_angle, R_EQUALS),
            rule_lit(3, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_distance, R_AND),
            rule_lit(3, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(2, data_distance, R_AND),
            rule_lit(4, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),

        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_distance, R_AND),
            rule_lit(0, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_distance, R_AND),
            rule_lit(1, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_distance, R_AND),
            rule_lit(2, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_distance, R_AND),
            rule_lit(3, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
        rule_alloc(FZ_MAMDANI, 3,
            rule_lit(3, data_distance, R_AND),
            rule_lit(4, data_angle, R_EQUALS),
            rule_lit(2, data_deviation, R_STOP)),
    };
    enum { rules_count = sizeof rules / sizeof rules[0] };

    time_t seed = time(NULL);
    srand(seed);

    SetTargetFPS(144);
    InitWindow(800, 800, "Mobile Robot");
    printf("seed: %ld\n", seed);

    Rectangle room = { .x = 100, .y = 100, .height = 600, .width = 600 };

    Robot rs[] = {
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        { .size = 16, .speed = { 100, 100 }, .pos = vec2_rand(200, 600) },
        // { .size = 16, .speed = { 0 }, .pos = vec2_rand(200, 600) },
        // { .size = 16, .speed = { 0 }, .pos = vec2_rand(200, 600) },
        // { .size = 16, .speed = { 0 }, .pos = vec2_rand(200, 600) },
        // { .size = 16, .speed = { 0 }, .pos = vec2_rand(200, 600) },
    };
    size_t rs_count = (sizeof(rs) / sizeof(rs[0]));

    Graph g = graph(10, DARKGRAY, BLACK, WHITE);
    g.show_legend = false;

    g.st_reset.pos = (Vector2) { 165, 195 };
    g.st_reset.scale = (Vector3) { 1, 1, 90 };
    g.st_current = g.st_reset;

    for (size_t i = 0; i < fs_count; i++) {
        Line* buf_line[cs[i]->count];

        int line_point_count = 101;

        for (size_t j = 0; j < cs[i]->count; j++) {
            buf_line[j] = line_alloc(&g,
                line_point_count, cs[i]->mfs[j].name, RED, 3);
        }

        double values[10];
        Array m = { .items = values };

        double point_pos = 0;
        for (int j = 0; j < line_point_count; j++) {
            m.count = cs[i]->count;
            fuzzy_forward(m, cs[i], fz_lerp(cs[i], point_pos));
            for (size_t k = 0; k < m.count; k++) {
                buf_line[k]->points[j] = (Vector2) {
                    point_pos, m.items[k] + (i * 2)
                };
            }
            point_pos += 0.01;
        }
    }

    bool graph_enabled = false;

    double ms_items[fs_count][10];
    Array ms[fs_count];
    for (size_t i = 0; i < fs_count; i++) {
        ms[i].count = cs[i]->count;
        ms[i].items = ms_items[i];
    }

    double res_items[cs[data_deviation]->count];
    Array res = {
        .items = res_items,
        .count = cs[data_deviation]->count,
    };

    bool calculate = true;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q))
            break;

        float delta = GetFrameTime();
        if (IsKeyDown(KEY_SPACE)) {
            delta /= 20.0;
        }
        if (IsKeyPressed(KEY_S)) {
            calculate = !calculate;
        }

        if (calculate) {
            for (size_t j = 0; j < rs_count; j++) {
                Robot* rb = &rs[j];
                for (size_t i = 0; i < rs_count; i++) {
                    if (i == j) {
                        continue;
                    }
                    float distance = Vector2Distance(rs[i].pos, rb->pos) / 100.0;
                    float angle = angle_between_points(rs[i].pos, rb->pos, Vector2Add(rb->pos, rb->speed));

                    logging("Data %zu: Distance = %.1lf | Angle = %.1lf\n", i, distance, angle);

                    for (size_t class = 0; class < fs_count - 1; class++) {
                        ms[class].count = cs[class]->count;

                        if (class == data_distance) {
                            fuzzy_forward(ms[class], cs[class], distance);
                            logging("%s:\n\t", "Distance");
                        } else if (class == data_angle) {
                            fuzzy_forward(ms[class], cs[class], angle);
                            logging("%s:\n\t", "Angle");
                        }
                        for (size_t j = 0; j < ms[class].count; j++) {
                            logging("%.2f, ", ms[class].items[j]);
                        }
                        logging("\n");
                    }

                    rule_forward(res, cs, ms, rules, rules_count);

                    double defuzz = defuzz_centroid(cs[data_deviation], res);

                    logging("res: ");
                    for (size_t i = 0; i < res.count; i++) {
                        logging("%.2f, ", res.items[i]);
                    }
                    logging("\n");

                    logging("Result: %.1lf ", defuzz);
                    if (!isnan(defuzz) && defuzz != 0) {
                        logging("TURNING");
                        rb->speed = Vector2Rotate(rb->speed, defuzz * delta / 10.0);
                    }
                    logging("\n");
                }

                Vector2 speed_scaled = Vector2Scale(rb->speed, delta);
                Vector2 new_pos = Vector2Add(rb->pos, speed_scaled);
                bool inside = CheckCollisionCircleRec(new_pos, 1, room);
                if (!inside) {
                    if (new_pos.x > room.x + room.width) {
                        rb->speed.x *= -1.0; //* (rand() / (float)RAND_MAX);
                    } else if (new_pos.x < room.x) {
                        rb->speed.x *= -1.0; //* (rand() / (float)RAND_MAX);
                    }
                    if (new_pos.y > room.y + room.height) {
                        rb->speed.y *= -1.0; //* (rand() / (float)RAND_MAX);
                    } else if (new_pos.y < room.y) {
                        rb->speed.y *= -1.0; //* (rand() / (float)RAND_MAX);
                    }
                } else {
                    rb->pos = new_pos;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleRec(room, DARKGRAY);

        for (size_t i = 0; i < (sizeof(rs) / sizeof(rs[0])); i++) {
            DrawLineV(rs[i].pos, Vector2Add(rs[i].pos, rs[i].speed), GREEN);
        }

        for (size_t i = 0; i < (sizeof(rs) / sizeof(rs[0])); i++) {
            bool is_colliding = false;
            for (size_t j = 0; j < (sizeof(rs) / sizeof(rs[0])); j++) {
                if (i == j) {
                    continue;
                }
                is_colliding = CheckCollisionCircles(rs[i].pos, rs[i].size, rs[j].pos, rs[i].size);
                if (is_colliding) {
                    DrawCircleV(rs[i].pos, rs[i].size, RED);
                    break;
                }
            }
            if (!is_colliding) {
                DrawCircleV(rs[i].pos, rs[i].size, GREEN);
            }
        }

        if (IsKeyPressed(KEY_G))
            graph_enabled = !graph_enabled;
        if (graph_enabled) {
            g.bound = (Rectangle) {
                GetScreenWidth() / 2.0 + 10, 10, GetScreenWidth() / 2.0 - 20, GetScreenHeight() - 20
            };
            graph_update(&g);
            graph_draw(&g);
        }

        EndDrawing();
    }

    for (size_t i = 0; i < rules_count; i++) {
        free(rules[i]);
    }

    for (size_t i = 0; i < fs_count; i++) {
        free(cs[i]);
    }

    graph_free(&g);

    CloseWindow();

    return 0;
}
