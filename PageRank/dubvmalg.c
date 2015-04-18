#include "dubvmalg.h"
#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <math.h>

double differce_vector_length(struct DubArray * x1,struct DubArray * x0)
{
	double result;
	struct DubArray temp = makecopy(x1);
	// printf("copying vector = \t");
	// print_DubArray(&temp);

	alphaxplusy_y(-1,x0,&temp);

	result = cblas_dasum(temp.size,temp.array,1);

	free(temp.array);

	return result;
}

struct DubArray makecopy(struct DubArray * a)
{
	struct DubArray temp = initialize_vector((int)(a->size),0);
	cblas_dcopy(a->size,a->array,1,temp.array,1);
	return temp;
}


double L1_difference(struct DubArray * x1, struct DubArray * x0)
{
	double result;
	result = cblas_dasum(x1->size,x1->array,1) - cblas_dasum(x0->size,x0->array,1);
	return result;
}

struct DubArray alphaAtimesx(struct DubArray * A,struct DubArray * x)
{
	struct DubArray y = initialize_vector((int)(x->size),0);

	int v_size = (int)(x->size);
	int M, N, LDA, INCX, INCY, ALPHA;
	double BETA = 0;
	M = N = LDA = v_size;
	INCX = INCY = ALPHA = 1;

	// printf("found apasdasd\n");

	cblas_dgemv(CblasRowMajor,CblasNoTrans,M,N,ALPHA,A->array,LDA,x->array,INCX,BETA,y.array,INCY);

	return y;
}

struct DubArray A_times_x_plus_y(struct DubArray * A,struct DubArray * x, struct DubArray * y)
{
	struct DubArray output = makecopy(y);

	int v_size = (int)(x->size);
	int M, N, LDA, INCX, INCY;
	double BETA = 1, ALPHA = 1;
	M = N = LDA = v_size;
	INCX = INCY = 1;

	// printf("found apasdasd\n");

	cblas_dgemv(CblasRowMajor,CblasNoTrans,M,N,ALPHA,A->array,LDA,x->array,INCX,BETA,output.array,INCY);

	return output;
}

void scale(struct DubArray * x, double scale)
{
	cblas_dscal(x->size, scale, x->array,1);
}

void alphaxplusy_y(double alpha, struct DubArray * x, struct DubArray * y)
{
	// y is altered to be alpha*x+y, x remains unaltered
	cblas_daxpy(x->size,alpha,x->array,1,y->array,1);
}

double dotproduct(struct DubArray * x, struct DubArray * y)
{
	return cblas_ddot((int)(x->size),x->array,1,y->array,1);
}

void alphaxtimesyTplusA(double alpha, struct DubArray * x, struct DubArray * y, struct DubArray * A)
{

	// note that in this function, we have result = alpha*x*transpose(y) + A
	int incx, incy, leny, lenx,lda;	

	incx = incy = 1;
	leny = lda = y->size;
	lenx = x->size;

	cblas_dger(CblasRowMajor,lenx,leny,alpha,x->array,incx,y->array,incy,A->array,lda);
}

void detect_converged(struct DubArray * before, struct DubArray * after, struct DubArray * converged, double epsilon, int ** C, 
	struct DubArray * A, int * count)
{
	int i;
	int n = (int)(before->size);
	struct DubArray test_after = makecopy(after);
	double difference = 0;

	int * to_zero = (int*)calloc(n,sizeof(int));
	int num_to_zero = 0;

	alphaxplusy_y(-1,before,&test_after);

	for(i = 0; i < test_after.size;i++)
	{
		if((*C)[i] != 1)
		{

			difference = fabs((test_after.array[i]) / (before->array[i]));
			if (difference < epsilon)
			{
				converged->array[i] = after->array[i];	// this element has converged
				(*C)[i] = 1;
				*count += 1;
				to_zero[num_to_zero++] = i+1;
			}
		}
	}

	zero_rows(A,to_zero);

	free(test_after.array);
	free(to_zero);
}

void detect_convergedMAPR(struct DubArray * before, struct DubArray * after, struct DubArray * converged, double epsilon, int ** C, 
	struct DubArray * A, struct DubArray * A_NN, struct DubArray * A_CN, int * count)
{
	int i;
	int n = (int)(before->size);
	struct DubArray test_after = makecopy(after);
	double difference = 0;

	int * to_zero = (int*)calloc(n,sizeof(int));
	int num_to_zero = 0;

	alphaxplusy_y(-1,before,&test_after);
	
	for(i = 0; i < test_after.size;i++)
	{
		if((*C)[i] != 1)
		{

			difference = fabs((test_after.array[i]) / (before->array[i]));
			if (difference < epsilon)
			{
				converged->array[i] = after->array[i];	// this element has converged
				(*C)[i] = 1;
				*count += 1;
				to_zero[num_to_zero++] = i+1;
			}
		}
	}

	// remove the rows and columns from A_NN as these have converged
	zero_rows(A_NN,to_zero);
	zero_cols(A_NN,to_zero);

	// the same rows that we want to zero for A_NN are the columns that we want to 
	// add from A for A_CN
	modify_CN(A, A_CN,n, to_zero,C);

	free(test_after.array);
	free(to_zero);
}

void modify_CN(struct DubArray * A, struct DubArray * A_CN, int n, int * cols_to_add, int ** C)
{
	// here we want to, in order,
	// Add the columns to A_CN from A that correspond to the values in cols_to_add
	// remove all the rows corresponding to the values of C that are 0

	int i;
	int * to_zero = (int*)calloc(n,sizeof(int));
	int num_to_zero = 0;

	//transpose to add columns as rows
	mtranspose(A,n);
	mtranspose(A_CN,n);

	// print_DubMatrix(A);
	for(i = 0; cols_to_add[i] != 0; i++)
	{
		cblas_dcopy(n,&(A->array[(cols_to_add[i]-1)*n]),1,&(A_CN->array[(cols_to_add[i]-1)*n]),1);
		// print_DubMatrix(A_CN);
		// sleep(1);
	}

	mtranspose(A,n);
	mtranspose(A_CN,n);

	//build the list of indices of elements that have converged
	for(i = 0; i < n; i++)
	{
		if((*C)[i] == 1)
		{
			to_zero[num_to_zero++] = i+1;
		}
	}

	//zero those rows
	zero_rows(A_CN,to_zero);

	free(to_zero);
}

void mtranspose(struct DubArray * A, int n)
{
	// A is the n by n matrix we will transpose

	double temp;	// hold the intermediate value as we switch the values in the elements

	int i,j;

	for(i = 0; i < n-1; i++)
	{
		for(j = i+1; j < n; j++)
		{
			temp = A->array[i*n+j];
			A->array[i*n+j] = A->array[j*n+i];
			A->array[j*n+i] = temp;
		}
	}

}

void zero_rows(struct DubArray * matrix, int * rows)
{
	// here we assume that the matrices are row major, meaning that 
	// each row of the matrix is listed serially in matrix. 
	// the values in rows are expected to be greater than 0, with
	// a value of zero signifying the end of the list
	int rowlen = (int)(sqrt((int)(matrix->size)));
	double * zeros = (double *)calloc(rowlen,sizeof(double));
	
	// printf("rowlen = %d\n", rowlen);
	int i;

	for(i = 0; rows[i] != 0;i++ )
	{
		// printf("here with row %d\n", rows[i]-1);
		//rows[i] contains the row number that we want to zero out
		cblas_dcopy(rowlen,zeros,1,&(matrix->array[(rows[i]-1)*rowlen]),1);
	}

	free(zeros);
}

void zero_cols(struct DubArray * matrix, int * cols)
{
	// here we assume that the matrices are row major, meaning that 
	// each row of the matrix is listed serially in matrix. 
	// the values in columns are expected to be greater than 0, with
	// a value of zero signifying the end of the list
	int n = (int)(sqrt(matrix->size));
	mtranspose(matrix, n);
	zero_rows(matrix,cols);
	mtranspose(matrix,n);
}