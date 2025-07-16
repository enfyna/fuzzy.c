#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#define MF_MAX_ARGS 4

#ifndef FZ_NO_LOGS
#define logging printf
#else
#define logging(...) ((void)0)
#endif

struct MF;
typedef double (*mf)(struct MF, double);

typedef struct MF {
    mf forward;
    double weight;
    size_t args_count;
    double args[MF_MAX_ARGS];
    const char* name;
} MF;

#define mf_forward(mf, value) (mf).forward((mf), value)

typedef struct {
    const char* name;
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

#define fz_gauss(name, c, s) (mf) mf_gauss, name, (double)c, (double)s
#define fz_trimf(name, a, b, c) (mf) mf_trimf, name, (double)a, (double)b, (double)c
#define fz_trapmf(name, a, b, c, d) (mf) mf_trapmf, name, (double)a, (double)b, (double)c, (double)d
#define fuzzy_alloc(name, class_count, start, end, ...) \
    fuzzy_alloc_null((const char*)name, (size_t)class_count, (double)start, (double)end, __VA_ARGS__, NULL)

Fuzzy* fuzzy_alloc_null(const char* name, size_t class_count, double bound_bot, double bound_top, ...);
void fuzzy_forward(Array dest, Fuzzy* fz, double value);
double fuzzy_defuzzify(Fuzzy* fz, Array weights);

// arg 0: a;
// arg 1: b;
// arg 2: c;
// arg 3: d;
double mf_trapmf(MF mf, double value);

// arg 0: a;
// arg 1: b;
// arg 2: c;
double mf_trimf(MF mf, double value);

// arg 0: c;
// arg 1: s;
double mf_gauss(MF mf, double value);

enum RuleOp {
    R_EQUALS,
    R_AND,
    R_OR,
    R_STOP,
};

static const char* rule_op_cstr[] = {
    "then",
    "and",
    "or",
    ".",
};

typedef struct {
    size_t idx_class;
    size_t idx_cluster;
    enum RuleOp op;
} RuleLit;

typedef struct {
    RuleLit expected[1];
    size_t count;
    RuleLit lits[];
} Rule;

#define rule_lit(idx_cluster, idx_class, op) \
    (size_t)idx_cluster, (size_t)idx_class, (size_t)op

Rule* rule_alloc(size_t lit_count, ...);
void rule_forward(Array dest, Fuzzy* fs[], Array* ms, Rule* rule[], size_t rule_count);
