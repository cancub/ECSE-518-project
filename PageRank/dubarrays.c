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

int compare_DubArrays(struct DubArray * a, struct DubArray * b)
{
	//return 1 if they are equal, 0 if they are not;
	int i,result = 1;
	if(a->size == b->size)
	{
		for(i = 0; i < a->size; i++)
		{
			if(a->array[i] != b->array[i])
			{
				result = 0;
				break;
			}
		}
	}

	return result;
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
			printf("%8.4f",a->array[i]);
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
