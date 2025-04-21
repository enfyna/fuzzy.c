#include "fuzzy.h"

double func_x(double value, double offset) { return value + offset; }
double func_x_mirror(double value, double offset) { return -value - offset; }
double func_zero(double value, double offset)
{
    (void)value;
    (void)offset;
    return 0.0;
}
double func_one(double value, double offset)
{
    (void)value;
    (void)offset;
    return 1.0;
}
double func_unit_triangle(double value, double offset)
{
    if (value < offset) {
        return func_x(value, -(offset - 1.0));
    } else {
        return func_x(-value, offset + 1.0);
    }
}
double func_unit_trapezoid(double value, double offset)
{
    if (value < offset) {
        return func_x(value, -(offset - 1.0));
    } else if (value < offset + 1.0) {
        return 1.0;
    } else {
        return func_x(-value, offset + 2.0);
    }
}

double func_forward(func g, double value)
{
    double val = g.func(value, g.offset);
    if (val > 1.0)
        return 1.0;
    if (val < 0.0)
        return 0.0;
    return val;
}

fuzzy* fuzzy_alloc_null(size_t class_count, double bound_bot, double bound_top, ...)
{
    assert(class_count > 0);
    assert(bound_bot < bound_top);

    fuzzy* fz = malloc(sizeof(fuzzy) + sizeof(*fz->funcs) * class_count);
    fz->count = class_count;
    fz->bounds[0] = bound_bot;
    fz->bounds[1] = bound_top;

    va_list ap;

    size_t count = 0;
    double (*func)(double value, double offset);

    va_start(ap, bound_top);
    while ((func = va_arg(ap, double (*)(double, double)))) {
        double offset = va_arg(ap, double);
        assert(count < class_count);
        fz->funcs[count].func = func;
        fz->funcs[count].offset = offset;
        count++;
    }
    va_end(ap);

    return fz;
}

void fuzzy_forward(memberships dest, fuzzy* fz, double value)
{
    assert(dest.count == fz->count);
    for (size_t i = 0; i < fz->count; i++) {
        dest.values[i] = func_forward(fz->funcs[i], value);
    }
}
