#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>

struct TwoDArray
{
	struct Array * array;
	size_t size;
};

struct Array
{
	int * array;
	size_t size;
};

struct DubArray
{
	double * array;
	size_t size;
};

struct SquareMat
{
	double ** array;
	size_t n;	
};

struct SquareMat initialize_graph(struct TwoDArray * a);
void print_matrix(struct SquareMat * M);
void add_array(struct TwoDArray * a);
void add_element(struct TwoDArray * a, int n, int value);
void construct_2DArray(struct TwoDArray * a);
void print_2DArray(struct TwoDArray * a);
struct Array get_PageRank(struct SquareMat * P);
void initialize_even_vector(struct DubArray * v, int size);
struct SquareMat xtimesy(struct DubArray * x, struct DubArray * y);
void obtain_graph_VE(char * filename, struct TwoDArray * a);

int main (){
	int i, j, max = 0;
	
	char filename[] = "test_graph.txt";
	struct SquareMat netgraph, test;
	struct TwoDArray temp_array;
	struct DubArray x,y;


	construct_2DArray(&temp_array);

	obtain_graph_VE(filename,&temp_array);

	printf("\n");

	// print_2DArray(&temp_array);

	netgraph = initialize_graph(&temp_array);

	print_matrix(&netgraph);

	printf("\n");

	x.size = 5;
	y.size = 5;

	x.array = (double *)malloc(x.size * sizeof(double));
	y.array = (double *)malloc(y.size * sizeof(double));

	for (i = 0; i < x.size; i++)
	{
		x.array[i] = i+1;
		y.array[i] = i+2;
	}

	test = xtimesy(&x,&y);

	print_matrix(&test);


	return 0;

}













// function definitions follow


// struct Array get_PageRank(struct SquareMat * P)
// {	
// 	struct DubArray start_v;
// 	initialize_even_vector(&start_v, (int)(P->n));



// }

void obtain_graph_VE(char * filename, struct TwoDArray * a)
{
	
	int i,j;
	FILE * ifp = fopen(filename,"r");

	if (ifp == NULL)
	{
		fprintf(stderr, "Cannot open file %s\n", filename);
	}
	else
	{
		while(fscanf(ifp, "%d %d", &i, &j) == 2)
		{
			if (i > a->size)
			{
				add_array(a);
			}

			add_element(a,i-1,j);

			// printf("i = %d, j = %d\n", i,j);
		}
	}

	fclose(ifp);
}

struct SquareMat xtimesy(struct DubArray * x, struct DubArray * y)
{
	struct SquareMat output;
	int i,j,k;
	int incx, incy, alpha, leny, lenx,lda;	
	int length = (x->size) * (y->size);
	double one_line_matrix[length];

	incx = incy = alpha = 1;
	leny = lda = y->size;
	lenx = x->size;

	j = k = 0;

	for(i = 0; i < length; i++)
	{
		one_line_matrix[i] = 0;
	}

	cblas_dger(CblasRowMajor,leny,lenx,alpha,x->array,incx,y->array,incy,one_line_matrix,lda);

	output.n = x->size;
	output.array = (double **)malloc(output.n * sizeof(double *));
	for(i = 0; i < output.n; i++)
	{
		output.array[i] = (double *)malloc(output.n * sizeof(double));

		for(j = 0; j < output.n; j++)
		{
			output.array[i][j] = one_line_matrix[k++];
		}
	}

	return output;
}

void initialize_even_vector(struct DubArray * v, int size)
{
	int i;
	v->size = size;
	v->array = (double *)malloc(size * sizeof(double));

	for(i = 0; i < size; i++)
	{
		v->array[i] = 1/((double)size);
	}
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

void construct_2DArray(struct TwoDArray * a)
{
	a->size = 0;
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

void print_matrix(struct SquareMat * M)
{
	int i,j;

	for(i = 0; i < M->n; i++)
	{
		for(j = 0; j < M->n; j++)
		{
			printf("%7.3f", M->array[i][j]);
		}

		printf("\n");
	}
}

struct SquareMat initialize_graph(struct TwoDArray * a)
{
	// initialize the web graph by using the 2D array we obtained from the text file
	int i,j,k, added;
	struct SquareMat matrix;

	matrix.n = a->size;

	// note that this is a square matrix, so we allocate memory for n elements of n rows;
	matrix.array = (double **)malloc(matrix.n * sizeof(double *));

	for(i = 0; i < matrix.n; i++)
	{
		matrix.array[i] = (double *)malloc(matrix.n * sizeof(double));

		for(j = 0; j< matrix.n; j++)
		{
			added = 0;

			for(k = 0; k < (a->array[i]).size; k++)
			{
				// if this column number exists in the i'th array in a, we
				// know that there is a directed link from i to j
				// and we assign the value 1/deg(i) to this square
				if(j + 1 == (a->array[i]).array[k])
				{
					// printf("match between %d and (a->array[i = %d]).array[k = %d] number\n", j+1,i,k);
					// printf("(a->array[i = %d].size = %d\n",i,(int)((a->array[i]).size) );
					matrix.array[i][j] = 1/(double)((a->array[i]).size);
					added = 1;
					break;
				}
			}

			// otherwise, if no vertex exists between i and j nodes, we enter a 0
			if(added == 0)
			{
				matrix.array[i][j] = 0;
			}
		}
	}

	return matrix;
}