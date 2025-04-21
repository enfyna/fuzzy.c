#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    double offset;
    double (*func)(double value, double offset);
} func;

typedef struct {
    size_t count;
    double bounds[2];
    func funcs[];
} fuzzy;

typedef struct {
    size_t count;
    double* values;
} memberships;

#define fuzzy_func(func, offset) func, (double)offset
#define fuzzy_alloc(class_count, bound_bot, bound_top, ...) fuzzy_alloc_null(class_count, bound_bot, bound_top, __VA_ARGS__, NULL)

fuzzy* fuzzy_alloc_null(size_t class_count, double bound_bot, double bound_top, ...);
void fuzzy_forward(memberships dest, fuzzy* fz, double value);

double func_forward(func g, double value);
double func_one(double value, double offset);
double func_zero(double value, double offset);
double func_x(double value, double offset);
double func_x_mirror(double value, double offset);
double func_unit_triangle(double value, double offset);
double func_unit_trapezoid(double value, double offset);
