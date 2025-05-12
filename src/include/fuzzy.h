#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#define MF_MAX_ARGS 7

typedef double (*mf)(double, double);

typedef struct {
    mf mf;
    double offset;
} MF;

struct MF2;
typedef double (*mf2)(struct MF2, double);
typedef struct MF2 {
    mf2 mf;
    size_t args_count;
    double args[MF_MAX_ARGS];
} MF2;

typedef struct {
    double bounds[2];
    size_t count;
    MF mfs[];
} Fuzzy;

typedef struct {
    size_t count;
    double* items;
} Array;

double norm(double min, double max, double val);
double denorm(double min, double max, double val);

#define fuzzy_mf(mfunc, offset) (mf) mfunc, (double)offset
#define fuzzy_alloc(class_count, bound_bot, bound_top, ...) fuzzy_alloc_null(class_count, bound_bot, bound_top, __VA_ARGS__, NULL)

Fuzzy* fuzzy_alloc_null(size_t class_count, double bound_bot, double bound_top, ...);
void fuzzy_forward(Array dest, Fuzzy* fz, double value);

double mf_forward(MF g, double value);
double mf_one(double value, double offset);
double mf_zero(double value, double offset);
double mf_x(double value, double offset);
double mf_x_mirror(double value, double offset);
double mf_cauchy(double value, double offset);
double mf_reverse_x(double value, double offset);
double mf_reverse_x_mirror(double value, double offset);
double mf_parabolica(double value, double offset);
double mf_unit_triangle(double value, double offset);
double mf_unit_trapezoid(double value, double offset);
double mf_unit_gaussian(double value, double offset);
double mf_trapezoid_5(double value, double offset);

double mf_forward_v2(MF2 mf, double value);

// arg 0: a;
// arg 1: b;
// arg 2: c;
double mf_trimf(MF2 mf, double value);

// arg 0: c;
// arg 1: s;
double mf_gauss(MF2 mf, double value);

#define rule_first_data 1
#define rule_first_membership 2
#define rule_second_data 3
#define rule_second_membership 4
#define rule_third_data 5
#define rule_third_membership 6

enum RuleOp {
    R_EQUALS,
    R_AND,
    R_OR,
    R_STOP,
};

enum RuleClass {
    R_VERY_LOW,
    R_LOW,
    R_MED,
    R_HIGH,
    R_VERY_HIGH,
};

static const char* rule_op_cstr[] = {
    "=",
    "&",
    "|",
    ".",
};

static const char* rule_class_cstr[] = {
    "VeryLow",
    "Low",
    "Med",
    "High",
    "VeryHigh",
};

typedef struct {
    size_t data_idx;
    enum RuleClass data_class;
    enum RuleOp op;
} RuleLit;

typedef struct {
    RuleLit expected[1];
    size_t count;
    RuleLit lits[];
} Rule;

#define rule_lit(data_class, data_id, op) (size_t)data_class, (size_t)data_id, (size_t)op
Rule* rule_alloc(size_t lit_count, ...);
void rule_forward(Array dest, Array* ms, Rule* rule[], size_t rule_count);
