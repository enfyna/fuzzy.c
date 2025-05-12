#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fuzzy.h"

double norm(double min, double max, double val)
{
    return (val - min) / (max - min);
}

double denorm(double min, double max, double val)
{
    return val * (max - min) + min;
}

double mf_x(double value, double offset) { return value + offset; }
double mf_x_mirror(double value, double offset) { return -(-value - offset); }
double mf_cauchy(double value, double offset)
{
    // \frac { 1 }
    // {
    // 1 +\operatorname { abs }
    // \left(\frac {\left(x - c\right) } { a }\right) ^ { 2b }
    // }
    // 2b = 10
    // a = 1
    return 1 / (1 + pow(fabs(value - offset), 10));
}
double mf_reverse_x(double value, double offset)
{

    if (value <= offset) {
        return 0;
    }
    return 1 / (-value + offset) + 1;
}

double mf_reverse_x_mirror(double value, double offset)
{
    if (value >= offset) {
        return 0;
    }
    return 1 / (value - offset) + 1;
}
double mf_parabolica(double value, double offset) { return -pow(value - offset, 2) + 1; }
double mf_zero(double value, double offset)
{
    (void)value;
    (void)offset;
    return 0.0;
}
double mf_one(double value, double offset)
{
    (void)value;
    (void)offset;
    return 1.0;
}
double mf_unit_gaussian(double value, double offset)
{
    // \sigma == 1
    return pow(M_E, -pow(value - offset, 2) / 4);
}
double mf_unit_triangle(double value, double offset)
{
    if (value < offset) {
        return mf_x(value, -(offset - 1.0));
    } else {
        return mf_x(-value, offset + 1.0);
    }
}
double mf_unit_trapezoid(double value, double offset)
{
    if (value < offset) {
        return mf_x(value, -(offset - 1.0));
    } else if (value < offset + 1.0) {
        return 1.0;
    } else {
        return mf_x(-value, offset + 2.0);
    }
}
double mf_trapezoid_5(double value, double offset)
{
    if (value < offset) {
        return mf_x(value, -(offset - 1.0));
    } else if (value < offset + 5.0) {
        return 1.0;
    } else {
        return mf_x(-value, offset + 6.0);
    }
}

double mf_forward(MF g, double value)
{
    double val = g.mf(value, g.offset);
    if (val > 1.0)
        return 1.0;
    if (val < 0.0)
        return 0.0;
    return val;
}

double mf_forward_v2(MF2 mf, double value)
{
    double val = mf.mf(mf, value);
    return val;
}

double mf_trimf(MF2 mf, double value)
{
    assert(mf.args_count == 3);
    double a = mf.args[0];
    double b = mf.args[1];
    double c = mf.args[2];
    if (value < a) {
        return 0;
    } else if (value < b) {
        return (value - a) / (b - a);
    } else if (value < c) {
        return (c - value) / (c - b);
    } else {
        return 0;
    }
}

double mf_gauss(MF2 mf, double value)
{
    assert(mf.args_count == 2);
    double c = mf.args[0];
    double s = mf.args[1];
    double s2 = pow(s, 2);
    double t = -pow(value - c, 2);
    return pow(M_E, t / (2 * s2));
}

Fuzzy* fuzzy_alloc_null(size_t class_count, double bound_min, double bound_max, ...)
{
    assert(class_count > 0);
    assert(bound_min < bound_max);

    Fuzzy* fz = malloc(sizeof(Fuzzy) + sizeof(*fz->mfs) * class_count);
    fz->count = class_count;
    fz->bounds[0] = bound_min;
    fz->bounds[1] = bound_max;

    va_list ap;

    size_t count = 0;
    mf f;

    va_start(ap, bound_max);

    while ((f = va_arg(ap, mf))) {
        double offset = va_arg(ap, double);
        assert(count < class_count);
        fz->mfs[count].mf = f;
        fz->mfs[count].offset = norm(bound_min, bound_max, offset);
        count++;
    }

    va_end(ap);

    assert(count == class_count);

    return fz;
}

void fuzzy_forward(Array dest, Fuzzy* fz, double value)
{
    assert(dest.count == fz->count);
    // double max = fz->bounds[1];
    // double min = fz->bounds[0];
    for (size_t i = 0; i < fz->count; i++) {
        dest.items[i] = mf_forward(fz->mfs[i], norm(fz->bounds[0], fz->bounds[1], value));
    }
}

Rule* rule_alloc(size_t lit_count, ...)
{
    assert(lit_count > 1);

    Rule* rule = malloc(sizeof(Rule) + sizeof(*rule->lits) * (lit_count - 1));
    rule->count = lit_count - 1;

    va_list ap;

    va_start(ap, lit_count);
    for (size_t count = 0; count < lit_count; count++) {
        size_t data_class = va_arg(ap, size_t);
        size_t data_id = va_arg(ap, size_t);
        size_t op = va_arg(ap, size_t);
        if (count == lit_count - 1) {
            assert(op == R_STOP
                && "[ERROR] Last rule op must be R_STOP!");
        } else if (count == lit_count - 2) {
            assert(op == R_EQUALS
                && "[ERROR] Before last rule op must be R_EQUALS!");
        } else {
            // assert(op != R_OR
            // && "[ERROR] R_OR IS NOT IMPLEMENTED!");
            assert(op != R_EQUALS
                && "[ERROR] R_EQUALS must be used only before the last op!");
            assert(op != R_STOP
                && "[ERROR] R_STOP must be used only as the last op!");
        }
        if (count == lit_count - 1) {
            rule->expected[0].data_class = data_class;
            rule->expected[0].data_idx = data_id;
            rule->expected[0].op = op;
        } else {
            rule->lits[count].data_class = data_class;
            rule->lits[count].data_idx = data_id;
            rule->lits[count].op = op;
        }
    }
    va_end(ap);

    return rule;
}

void rule_forward(Array dest, Array* ms, Rule* rule[], size_t rule_count)
{
    for (size_t i = 0; i < dest.count; i++) {
        dest.items[i] = 0;
    }
    printf("\n");
    for (size_t i = 0; i < rule_count; i++) {
        Rule* rl = rule[i];

        double act = 0;
        enum RuleOp current_op;

        printf("%2zu: ", i + 1);
        for (size_t j = 0; j < rl->count; j++) {
            size_t data_id = rl->lits[j].data_idx;
            size_t data_class = rl->lits[j].data_class;
            size_t op = rl->lits[j].op;
            if (j > 0) {
                if (current_op == R_AND && ms[data_id].items[data_class] < act) {
                    act = ms[data_id].items[data_class];
                } else if (current_op == R_OR && ms[data_id].items[data_class] > act) {
                    act = ms[data_id].items[data_class];
                }
            } else {
                act = ms[data_id].items[data_class];
            }
            current_op = op;
            printf("%zu %s (%.2f) %s ", data_id, rule_class_cstr[data_class], ms[data_id].items[data_class], rule_op_cstr[op]);
        }
        dest.items[rl->expected[0].data_class] += act;
        printf("%zu %s (%.2f)\n", rl->expected[0].data_idx, rule_class_cstr[rl->expected[0].data_class], act);
    }
    printf("=============\n");
}
