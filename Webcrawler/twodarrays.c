#include "twodarrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int search_Array(struct Array a, int value)
{
	int i, result = 0;

	for (i = 0; i < a.size; i++)
	{
		if(a.array[i] == value)
		{
			result = 1;
			break;
		}
	}

	return result;
}

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

void print_Array(struct Array a)
{
	int i;

	printf("[");

	for(i = 0; i < a.size; i++)
	{
		printf("%d",a.array[i] );
		if(i < a.size -1)
		{
			printf(", ");
		}
	}
	printf("]");
}

void construct_2DArray(struct TwoDArray * a)
{
	a->size = 1;
	a->array = (struct Array *)malloc(sizeof(struct Array));
	(a->array[0]).array = (int *)malloc(sizeof(int));
	(a->array[0]).size = 0;
}

void construct_Array(struct Array * a)
{
	a->size = 0;
	a->array = (int *)malloc(sizeof(int));
	a->array[0] = -1;
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

void add_2Delement(struct TwoDArray * a, int n, int value)
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

void add_element(struct Array * a, int value)
{

	if (a->size > 0)
	{
		a->array = (int *)realloc(a->array, (sizeof(int))*(a->size +1));
	}

	a->array[a->size] = value;
	a->size += 1;
}