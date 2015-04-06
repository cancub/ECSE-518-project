#include "twodarrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


void print_2DArray(struct TwoDArray * a)
{
	int i,j;

	for (i = 0; i < a->size; i++)
	{
		printf("%d: ",i+1);
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

void destruct_2DArray(struct TwoDArray * a)
{
	int i;

	for(i = 0; i < a->size; i++)
	{
		free((a->array[i]).array);
	}

	free(a->array);
}

void add_array(struct TwoDArray * a)
{
	

	// printf("a->size = %d\n", (int)(a->size));
	if(a->size >= 1)
	{
		a->size += 1;
		// printf("a->size = %d\n",(int)(a->size) );
		a->array = (struct Array *)realloc(a->array, a->size * sizeof(struct Array));
		// printf("here\n");
		(a->array[a->size-1]).array = (int *)malloc(sizeof(int));
		(a->array[a->size-1]).size = 0;

		// printf("a->array[a->size-1 = %d]).size = %d\n", (int)(a->size-1), (int)((a->array[a->size-1]).size));
	}
	else
	{
		a->size = 1;
		// printf("a->array[a->size-1 = %d]).size = %d\n", (int)(a->size-1), (int)((a->array[a->size-1]).size));
	}

	// printf("\n\n");
}

void add_element(struct TwoDArray * a,int from, int value)
{
	struct Array * pArray = &((a->array[from]));

	if(pArray->size >= 1)
	{	
		pArray->size += 1;
		pArray->array = (int *)realloc(pArray->array,(int)(pArray->size) * sizeof(int));
		(pArray->array)[(int)(pArray->size - 1)] = value;
	}
	else
	{
		// printf("to = %d\n",value );
		(pArray->array)[pArray->size] = value;
		// printf("((a->array[from])->array)[(a->array[from])->size = %d] = %d\n",(int)((a->array[from])->size),((a->array[from])->array)[(a->array[from])->size]);
		pArray->size += 1;
		// printf("(a->array[from])->size = %d\n",(int)((a->array[from])->size) );
	}

	// printf("pointer[%d] = %d\n", (int)(pArray->size), (pArray->array)[(int)(pArray->size) -1] );

	// free(pArray);

}