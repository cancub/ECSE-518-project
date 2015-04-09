#ifndef TWODARRAYS_H_INCLUDED
#define TWODARRAYS_H_INCLUDED

#include <stddef.h>

struct Array
{
	int * array;
	size_t size;
};

struct TwoDArray
{
	struct Array * array;
	size_t size;
};

void add_array(struct TwoDArray * a);
void add_2Delement(struct TwoDArray * a, int n, int value);
void add_element(struct Array * a, int value);
void construct_2DArray(struct TwoDArray * a);
void construct_Array(struct Array * a);
void print_2DArray(struct TwoDArray * a);
void print_Array(struct Array a);
int search_Array(struct Array a, int value);
void print_2DArray_to_file(struct TwoDArray * a, char * location);
void delete_2DArray(struct TwoDArray * a);

#endif