#ifndef DUBVMALG_H_INCLUDED
#define DUBVMALG_H_INCLUDED

#include "dubarrays.h"



void alphaxtimesyTplusA(double alpha, struct DubArray * x, struct DubArray * y, struct DubArray * A);
double dotproduct(struct DubArray * x, struct DubArray * y);
void alphaxplusy_y(double alpha, struct DubArray * x, struct DubArray * y);
void scale(struct DubArray * x, double scale);
struct DubArray alphaATtimesx(double ALPHA, struct DubArray * A,struct DubArray * x);
double differce_vector_length(struct DubArray * x1,struct DubArray * x0);
struct DubArray makecopy(struct DubArray * a);
double L1_difference(struct DubArray * x1, struct DubArray * x0);
void detect_converged(struct DubArray * before, struct DubArray * after, double epsilon, int ** C, struct DubArray * A);
// void mtranspose(struct DubArray * a);

#endif