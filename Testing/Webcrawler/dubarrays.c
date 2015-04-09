#include "dubarrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void print_DubMatrix(struct DubArray * a, int row_length)
{
	int i;
	for(i = 0; i < a->size; i++)
	{
		if(i%row_length == 0)
		{
			printf("\n");
		}
		
		if (a->array[i] == (int)(a->array[i]))
		{
			printf("%8d",(int)(a->array[i]));
		}
		else
		{
			printf("%8.2f",a->array[i]);
		}

	}

	printf("\n");
}

void print_DubArray(struct DubArray * a)
{
	int i;
	for(i = 0; i < a->size; i++)
	{	
		if ((a->array[i] > 0.9999) && (a->array[i] < 1.0001))
		{
			printf("%8d",1);
		}
		else
		{
			printf("%8.2f",a->array[i]);
		}

	}

	printf("\n");
}

struct DubArray initialize_vector(int length, double value)
{
	// printf("here1\n");
	struct DubArray temp;
	int i;
	if (length > 0)
	{
		temp.size = length;
		temp.array = (double *)malloc(temp.size * sizeof(double));
	}
	else
	{
		temp.size = 0;
		temp.array = (double *)malloc(sizeof(double));
	}

	for(i = 0; i < length; i++)
	{
		temp.array[i] = value;
	}

	return temp;
}
