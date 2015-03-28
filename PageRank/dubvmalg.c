#include "dubvmalg.h"
#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

double differce_vector_length(struct DubArray * x1,struct DubArray * x0)
{
	struct DubArray temp = makecopy(x1);
	// printf("copying vector = \t");
	// print_DubArray(&temp);

	alphaxplusy_y(-1,x0,&temp);

	return cblas_dasum(temp.size,temp.array,1);
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

struct DubArray alphaATtimesx(double ALPHA, struct DubArray * A,struct DubArray * x)
{
	struct DubArray y = initialize_vector((int)(x->size),0);

	int v_size = (int)(x->size);
	int M, N, LDA, INCX, INCY;
	double BETA = 0;
	M = N = LDA = v_size;
	INCX = INCY = 1;


	cblas_dgemv(CblasRowMajor,CblasTrans,M,N,ALPHA,A->array,LDA,x->array,INCX,BETA,y.array,INCY);

	return y;
}

void scale(struct DubArray * x, double scale)
{
	cblas_dscal(x->size, scale, x->array,1);
}

void alphaxplusy_y(double alpha, struct DubArray * x, struct DubArray * y)
{
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