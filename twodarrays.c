#include "twodarrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


void print_2DArray(struct TwoDArray * a)
{
	int i,j;

	for (i = 0; i < a->size; i++)
	{
		for(j = 0; j < (a->array[i]).size; j++)
		{
			printf("%3d", (a->array[i]).array[j]);
		}
		printf("\n");
	}
}

void construct_2DArray(struct TwoDArray * a)
{
	a->size = 1;
	a->array = (struct Array *)malloc(sizeof(struct Array));
	(a->array[0]).array = (int *)malloc(sizeof(int));
	(a->array[0]).size = 0;
}

void add_array(struct TwoDArray * a)
{

	if(a->size >= 1)
	{
		a->size += 1;
		a->array = (struct Array *)realloc(a->array, a->size * sizeof(struct Array));
		(a->array[a->size - 1]).array = (int *)malloc(sizeof(int));
		(a->array[a->size - 1]).size = 0;
	}
	else
	{
		a->size = 1;
	}
}

void add_element(struct TwoDArray * a, int n, int value)
{
	// printf("a->array[n = %d]).size = %d\n", n, (int)((a->array[n]).size));

	if((a->array[n]).size >= 1)
	{	
		(a->array[n]).size += 1;
		(a->array[n]).array = (int *)realloc((a->array[n]).array,(a->array[n]).size * sizeof(int));
		(a->array[n]).array[(a->array[n]).size - 1] = value;
	}
	else
	{
		(a->array[n]).array[(a->array[n]).size] = value;
		(a->array[n]).size += 1;	
	}

	// printf("a->array[n = %d].size = %d, value = %d\n",n,(int)((a->array[n]).size),value );
}