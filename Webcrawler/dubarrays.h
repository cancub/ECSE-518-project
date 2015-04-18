#ifndef DUBARRAYS_H_INCLUDED
#define DUBARRAYS_H_INCLUDED

#include <stddef.h>

struct DubArray
{
	double * array;
	size_t size;
};

// struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
// double dotproduct(struct DubArray * x, struct DubArray * y);
struct DubArray initialize_vector(int length, double value);
void print_DubArray(struct DubArray * a);
void print_DubMatrix(struct DubArray * a);
// void free_DubArray(struct DubArray * a);

#endif