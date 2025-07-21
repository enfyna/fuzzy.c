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

double lerp(double min, double max, double v)

{
    return (max - min) * v + min;
}

// https://www.mathworks.com/help/fuzzy/trapezoidalmf.html
double mf_trapmf(MF mf, double value)
{
    assert(mf.args_count == 4);
    double a = mf.args[0];
    double b = mf.args[1];
    double c = mf.args[2];
    double d = mf.args[3];
    double val = fmin(mf.weight, fmin((d - value) / (d - c), (value - a) / (b - a)));
    return val > 0 ? val : 0;
}

// https://www.mathworks.com/help/fuzzy/triangularmf.html
double mf_trimf(MF mf, double value)
{
    assert(mf.args_count == 3);
    double a = mf.args[0];
    double b = mf.args[1];
    double c = mf.args[2];
    if (value < a) {
        return 0;
    } else if (value < b) {
        return fmin(mf.weight, (value - a) / (b - a));
    } else if (value < c) {
        return fmin(mf.weight, (c - value) / (c - b));
    } else {
        return 0;
    }
}

// https://www.mathworks.com/help/fuzzy/gaussianmf.html
double mf_gauss(MF mf, double value)
{
    assert(mf.args_count == 2);
    double c = mf.args[0];
    double s = mf.args[1];
    double s2 = pow(s, 2);
    double t = -pow(value - c, 2);
    return fmin(mf.weight, exp(t / (2 * s2)));
}

Fuzzy* fuzzy_alloc_null(const char* name, enum FuzzyType type, size_t class_count, double bound_min, double bound_max, ...)
{
    assert(class_count > 0);
    assert(bound_min < bound_max);

    Fuzzy* fz = malloc(sizeof(Fuzzy) + sizeof(*fz->mfs) * class_count);
    fz->name = name;
    fz->count = class_count;
    fz->type = type;
    fz_min(fz) = bound_min;
    fz_max(fz) = bound_max;

    va_list ap;

    size_t count = 0;
    mf f;

    va_start(ap, bound_max);

    while ((f = va_arg(ap, mf))) {
        assert(count < class_count);
        fz->mfs[count].forward = f;
        fz->mfs[count].weight = 1.0;
        fz->mfs[count].name = va_arg(ap, const char*);
        if (type == FZ_SUGENO) {
            fz->mfs[count].scoef = va_arg(ap, double);
        }
        if (f == mf_trimf) {
            fz->mfs[count].args_count = 3;
            fz->mfs[count].args[0] = va_arg(ap, double);
            fz->mfs[count].args[1] = va_arg(ap, double);
            fz->mfs[count].args[2] = va_arg(ap, double);
        } else if (f == mf_trapmf) {
            fz->mfs[count].args_count = 4;
            fz->mfs[count].args[0] = va_arg(ap, double);
            fz->mfs[count].args[1] = va_arg(ap, double);
            fz->mfs[count].args[2] = va_arg(ap, double);
            fz->mfs[count].args[3] = va_arg(ap, double);
        } else if (f == mf_gauss) {
            fz->mfs[count].args_count = 2;
            fz->mfs[count].args[0] = va_arg(ap, double);
            fz->mfs[count].args[1] = va_arg(ap, double);
        } else {
            if (type == FZ_SUGENO) {
                fprintf(stderr,
                    "[ERROR] To create clusters for sugeno you need to use "
                    "the sugeno macros like : fz_trimf_sugeno()\n");
            }
            assert(false && "MF type undefined!");
        }
        count++;
    }

    va_end(ap);

    assert(count == class_count);

    return fz;
}

void fuzzy_forward(Array dest, Fuzzy* fz, double value)
{
    assert(dest.count == fz->count);
    for (size_t i = 0; i < fz->count; i++) {
        dest.items[i] = mf_forward(fz->mfs[i], value);
    }
}

double defuzz_centroid(Fuzzy* fz, Array weights)
{
    double result_top = 0;
    double result_bot = 0;
    assert(fz->count == weights.count);

    for (size_t i = 0; i < fz->count; i++) {
        fz->mfs[i].weight = weights.items[i];
    }

    Array dest = { 0 };
    dest.count = fz->count;
    dest.items = calloc(fz->count, sizeof(double));

    double point_pos = 0.0; // riemann sum
    for (size_t i = 0; i < 1000; i++) {
        double normal = fz_lerp(fz, point_pos);
        fuzzy_forward(dest, fz, normal);
        double m = 0.0;
        for (size_t j = 0; j < dest.count; j++) {
            m = fmax(m, dest.items[j]);
        }
        if (m != 0.0) {
            result_top += (m * point_pos);
            result_bot += m;
        }
        point_pos += 0.001;
    }

    for (size_t i = 0; i < fz->count; i++) {
        fz->mfs[i].weight = 1.0;
    }

    double result = fz_lerp(fz, result_top / result_bot);
    return result;
}

Rule* rule_alloc(enum FuzzyType type, size_t lit_count, ...)
{
    size_t ptr_size = sizeof(Rule);

    if (type == FZ_MAMDANI) {
        assert(lit_count > 1);
        ptr_size += sizeof(RuleLit) * (lit_count - 1);
    } else if (type == FZ_SUGENO) {
        assert(lit_count > 0);
        ptr_size += sizeof(RuleLit) * lit_count;
    } else {
        assert(false && "Invalid fuzzy type! [FZ_MAMDANI | FZ_SUGENO]");
    }

    Rule* rule = calloc(1, ptr_size);
    rule->count = lit_count - 1;
    rule->type = type;

    va_list ap;

    va_start(ap, lit_count);
    for (size_t count = 0; count < lit_count; count++) {
        size_t data_cluster = va_arg(ap, size_t);
        size_t data_class = va_arg(ap, size_t);
        size_t op = va_arg(ap, size_t);
        if (count == lit_count - 1) {
            assert(op == R_STOP
                && "[ERROR] Last rule op must be R_STOP!");
        } else if (type == FZ_MAMDANI && count == lit_count - 2) {
            assert(op == R_EQUALS
                && "[ERROR] Before last rule op must be R_EQUALS!");
        } else {
            assert(op != R_EQUALS
                && "[ERROR] R_EQUALS must be used only before the last op!");
            assert(op != R_STOP
                && "[ERROR] R_STOP must be used only as the last op!");
        }
        if (type == FZ_MAMDANI && count == lit_count - 1) {
            rule->expected[0].idx_cluster = data_cluster;
            rule->expected[0].idx_class = data_class;
            rule->expected[0].op = op;
        } else {
            rule->lits[count].idx_cluster = data_cluster;
            rule->lits[count].idx_class = data_class;
            rule->lits[count].op = op;
        }
    }
    va_end(ap);

    return rule;
}

void rule_forward(Array dest, Fuzzy* fs[], Array* ms, Rule* rs[], size_t rule_count)
{
    if (fs[0]->type == FZ_SUGENO) {
        assert(dest.count == rule_count);
    } else if (fs[0]->type == FZ_MAMDANI) {
        size_t class = rs[0]->expected[0].idx_class;
        assert(dest.count == fs[class]->count);
    }
    memset(dest.items, 0, sizeof(double) * dest.count);

    enum FuzzyType type;

    for (size_t i = 0; i < rule_count; i++) {
        Rule* rule = rs[i];

        if (i == 0) {
            type = rule->type;
        } else {
            assert(type == rule->type && "Dont mix sugeno and mamdani rules!");
        }

        double act = 0;

        enum RuleOp current_op;

        if (fs[0]->type == FZ_MAMDANI) {

            logging("%2zu: if ", i + 1);
            for (size_t j = 0; j < rule->count; j++) {
                size_t idx_class = rule->lits[j].idx_class;
                size_t idx_cluster = rule->lits[j].idx_cluster;
                size_t op = rule->lits[j].op;

                assert(idx_cluster < ms[idx_class].count);
                double val = ms[idx_class].items[idx_cluster];

                if (j == 0
                    || (current_op == R_AND && val < act)
                    || (current_op == R_OR && val > act)) {
                    act = val;
                }

                current_op = op;
                logging("%s is %s(%.2f) %s ", fs[idx_class]->name, fs[idx_class]->mfs[idx_cluster].name, val, rule_op_cstr[op]);
            }

            RuleLit expected = rule->expected[0];
            dest.items[expected.idx_cluster] = fmax(dest.items[expected.idx_cluster], act);
            logging("%s is %s(%.2f)\n", fs[expected.idx_class]->name, fs[expected.idx_class]->mfs[expected.idx_cluster].name, act);

        } else if (fs[0]->type == FZ_SUGENO) {
            logging("R%zu: \n  w%zu = ", i + 1, i + 1);

            for (size_t j = 0; j <= rule->count; j++) {
                size_t idx_class = rule->lits[j].idx_class;
                size_t idx_cluster = rule->lits[j].idx_cluster;
                size_t op = rule->lits[j].op;

                assert(idx_cluster < ms[idx_class].count);
                double val = ms[idx_class].items[idx_cluster];

                if (j == 0) {
                    act = val;
                } else if (current_op == R_AND) {
                    act *= val;
                } else if (current_op == R_OR) {
                    act = act + val - (act * val); // probabilistic or
                }

                current_op = op;
                logging("%s['%s'](%.2f)", fs[idx_class]->name, fs[idx_class]->mfs[idx_cluster].name, val);
                if (j < rule->count) {
                    logging(" %s ", rule_op_cstr[current_op]);
                }
            }
            logging(" = %.2lf\n", act);
            dest.items[i] = act;
        } else {
            assert(false && "Invalid fuzzy type! [FZ_MAMDANI | FZ_SUGENO]\n");
        }
    }
    logging("=============\n");
}
