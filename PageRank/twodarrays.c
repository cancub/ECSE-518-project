#include "twodarrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


void print_2DArray(struct TwoDArray * a)
{
	int i,j;

	for (i = 0; i < a->size; i++)
	{
		printf("%d: ",i);
		if((a->array[i]).size != 0)
		{
			for(j = 0; j < (a->array[i]).size; j++)
			{
				printf("%d ", (a->array[i]).array[j]);
			}
		}
		else
		{
			printf("(empty)");
		}
		printf("\n");
	}
}

void construct_2DArray(struct TwoDArray * a)
{
	a->size = 0;
	a->array = (struct Array *)malloc(sizeof(struct Array));
	(a->array[0]).array = (int *)malloc(sizeof(int));
	(a->array[0]).size = 0;
}

void add_array(struct TwoDArray * a)
{
	

	// printf("a->size = %d\n", (int)(a->size));
	if(a->size >= 1)
	{
		a->size += 1;
		// printf("a->size = %d\n",(int)(a->size) );
		// printf("here\n");
		a->array = (struct Array *)realloc(a->array, a->size * sizeof(struct Array));
		(a->array[a->size-1]).array = (int *)malloc(sizeof(int));
		(a->array[a->size-1]).size = 0;
		// printf("a->array[a->size-1 = %d]).size = %d\n", (int)(a->size-1), (int)((a->array[a->size-1]).size));
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
		// printf("from = %d, to = %d\n",n,value );
		(a->array[n]).array[(a->array[n]).size] = value;
		(a->array[n]).size += 1;	
	}

	// printf("a->array[n = %d].size = %d, value = %d\n",n,(int)((a->array[n]).size),value );
}