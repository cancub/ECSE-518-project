#ifndef DUBVMALG_H_INCLUDED
#define DUBVMALG_H_INCLUDED

#include "dubarrays.h"

void alphaxtimesyTplusA(double alpha, struct DubArray * x, struct DubArray * y, struct DubArray * A);
double dotproduct(struct DubArray * x, struct DubArray * y);
void alphaxplusy_y(double alpha, struct DubArray * x, struct DubArray * y);
void scale(struct DubArray * x, double scale);
struct DubArray alphaAtimesx(struct DubArray * A,struct DubArray * x);
struct DubArray A_times_x_plus_y(struct DubArray * A,struct DubArray * x, struct DubArray * y);
double differce_vector_length(struct DubArray * x1,struct DubArray * x0);
struct DubArray makecopy(struct DubArray * a);
double L1_difference(struct DubArray * x1, struct DubArray * x0);
void detect_converged(struct DubArray * before, struct DubArray * after, struct DubArray * converged, double epsilon, int ** C,
 struct DubArray * A, int * count);
void detect_convergedMAPR(struct DubArray * before, struct DubArray * after, struct DubArray * converged, double epsilon, int ** C,
 struct DubArray * A, int * count);
void zero_cols(struct DubArray * matrix, int * cols);
void zero_rows(struct DubArray * matrix, int * rows);
void mtranspose(struct DubArray * A, int n);


#endif