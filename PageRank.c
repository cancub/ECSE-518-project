#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
void print_matrix(struct SquareMat * M);
void add_array(struct TwoDArray * a);
void add_element(struct TwoDArray * a, int n, int value);
void construct_2DArray(struct TwoDArray * a);
void print_2DArray(struct TwoDArray * a);
struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_0, struct DubArray * v);
void alphaxtimesyTplusA(double alpha, struct DubArray * x, struct DubArray * y, struct DubArray * A);
void obtain_graph_VE(char * filename, struct TwoDArray * a);
double dotproduct(struct DubArray * x, struct DubArray * y);
struct DubArray initialize_vector(int length, double value);
void print_DubArray(struct DubArray * a);
void free_DubArray(struct DubArray * a);
void xplusytoy(struct DubArray * x, struct DubArray * y);
void scale(struct DubArray * x, double scale);

int main (){
	int i, j, max = 0;
	
	char filename[] = "test_graph.txt";
	struct SquareMat test;
	struct TwoDArray temp_array;
	struct DubArray x_0, start_v, result;

	construct_2DArray(&temp_array);

	obtain_graph_VE(filename,&temp_array);

	start_v = initialize_vector((int)(temp_array.size), 1/(double)(temp_array.size));
	x_0 = initialize_vector((int)(temp_array.size), 1/(double)(temp_array.size));

	result = get_PageRank(&temp_array, &x_0, &start_v);

	// print_2DArray(&temp_array);

	// print_matrix(&netgraph);

	// printf("\n");

	// x.size = 5;
	// y.size = 5;

	// x.array = (double *)malloc(x.size * sizeof(double));
	// y.array = (double *)malloc(y.size * sizeof(double));

	// for (i = 0; i < x.size; i++)
	// {
	// 	x.array[i] = i+1;
	// 	y.array[i] = i+2;
	// }

	// test = xtimesy(&x,&y);

	// printf("result = %8.2f\n", dotproduct(&x,&y));

	// print_matrix(&test);


	return 0;

}













// function definitions follow


struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_0, struct DubArray * v)
{	
	struct DubArray d,P,ones;
	int v_size,m_size; 
	double c = 0.85;

	v_size = (int)(x_0->size);
	m_size = (int)(pow(v_size,2));
	ones = initialize_vector(v_size,1);
	// printf("here-1\n");
	print_2DArray(G);
	P = initialize_graph(G,&d);
	print_DubArray(&P);
	scale(&P,c);
	printf("here\n");
	alphaxtimesyTplusA(c,&d,v,&P);
	printf("here2\n");
	alphaxtimesyTplusA(1-c,&ones,v,&P);

	// free(d.array);

	// print_DubArray(&d);
	// print_DubArray(v);
	print_DubArray(&P);
	// print_DubArray(&D);

	// printf("here\n");

	return d;


}

void print_DubArray(struct DubArray * a)
{
	int i;
	for(i = 0; i < a->size; i++)
	{
		printf("%8.2f",a->array[i]);
	}

	printf("\n");
}

void scale(struct DubArray * x, double scale)
{
	cblas_dscal(x->size, scale, x->array,1);
}

void xplusytoy(struct DubArray * x, struct DubArray * y)
{
	cblas_daxpy(x->size,1,x->array,1,y->array,1);
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

double dotproduct(struct DubArray * x, struct DubArray * y)
{
	return cblas_ddot((int)(x->size),x->array,1,y->array,1);
}

void obtain_graph_VE(char * filename, struct TwoDArray * a)
{
	
	int i,j,k, difference;
	FILE * ifp = fopen(filename,"r");

	if (ifp == NULL)
	{
		fprintf(stderr, "Cannot open file %s\n", filename);
	}
	else
	{
		while(fscanf(ifp, "%d %d", &i, &j) == 2)
		{

			if (i > (a->size))
			{
				difference = i - (a->size);


				for(k= 0; k < difference; k++)
				{
					add_array(a);
				}
			}

			// printf("a->size = %d\n", (int)(a->size) );			

			add_element(a,i-1,j);

			// printf("i = %d, j = %d\n", i,j);
		}
	}

	fclose(ifp);
}

void alphaxtimesyTplusA(double alpha, struct DubArray * x, struct DubArray * y, struct DubArray * A)
{

	// note that in this function, we have result = alpha*x*transpose(y) + A
	int incx, incy, leny, lenx,lda;	
	int length = (x->size) * (y->size);

	incx = incy = 1;
	leny = lda = y->size;
	lenx = x->size;

	cblas_dger(CblasRowMajor,lenx,leny,alpha,x->array,incx,y->array,incy,A->array,lda);

	// output.n = x->size;
	// output.array = (double **)malloc(output.n * sizeof(double *));
	// for(i = 0; i < output.n; i++)
	// {
	// 	output.array[i] = (double *)malloc(output.n * sizeof(double));

	// 	for(j = 0; j < output.n; j++)
	// 	{
	// 		output.array[i][j] = one_line_matrix[k++];
	// 	}
	// }
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

struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out)
{
	// initialize the web graph by using the 2D array we obtained from the text file
	int i,j, degree, added, k = 0;
	struct DubArray matrix_in_array;

	matrix_in_array.size = pow(a->size,2.0);

	// note that this is a square matrix, so we allocate memory for n elements of n rows;
	matrix_in_array.array = (double *)malloc(matrix_in_array.size * sizeof(double));

	for(i = 0; i < a->size; i++)
	{

		degree = 1;

		for(j = 0; j< a->size; j++)
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
					matrix_in_array.array[i * ((int)(a->size)) + j] = 1/(double)((a->array[i]).size);
					added = 1;
					degree = 0;
					break;
				}
			}

			// otherwise, if no vertex exists between i and j nodes, we enter a 0
			if(added == 0)
			{

				matrix_in_array.array[i * ((int)(a->size)) + j] = 0;
			}
		}

		no_out->array[i] = degree;
	}

	return matrix_in_array;
}