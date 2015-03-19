#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "dubarrays.h"
#include "twodarrays.h"
#include "dubvmalg.h"

struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_0, struct DubArray * v, int * iter);
void obtain_graph_VE(char * filename, struct TwoDArray * a);


int main (){
	int i, j, max = 0;
	
	char filename[15] = "test_graphx.txt";
	struct TwoDArray temp_array;
	struct DubArray x_0, start_v, result;
	char filenum;
	char * x_type;
	double x_vals,fraction;
	int itercount = 0;

	construct_2DArray(&temp_array);

	printf("Test graph to use:  ");
	scanf("%c", &filenum);
	printf("Type of starting x <e>ven/<o>nes: ");
	scanf("%s", x_type);

	filename[10] = filenum;

	obtain_graph_VE(filename,&temp_array);

	fraction = 1/(double)(temp_array.size);

	if (strcmp(x_type,"e") > 0)
	{
		x_vals = 1;
	}
	else
	{
		x_vals = fraction;
	}

	start_v = initialize_vector((int)(temp_array.size), fraction );
	x_0 = initialize_vector((int)(temp_array.size), x_vals);

	clock_t start = clock(), diff;
	result = get_PageRank(&temp_array, &x_0, &start_v, &itercount);
	diff = clock() - start;
	int msec = diff*1000/CLOCKS_PER_SEC;

	printf("\nResult = \n");
	print_DubArray(&result);

	printf("Time to converge = %d.%d s\n", msec/1000,msec%1000 );
	printf("Iterations to converge = %d\n", itercount);

	return 0;

}







// functions


struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_before, struct DubArray * v, int * iter)
{	
	struct DubArray d,P,ones,x_after;
	int v_size,m_size,i,k,j = 0; 
	double w, delta, c, epsilon = 1;
	char verbose[3];

	printf("Input damping factor c: ");
	scanf("%lf", &c);
	printf("Test value epsilon = 1x10^-");
	scanf("%d", &j);
	printf("Verbose? <y/n>: ");
	scanf("%s", verbose);

	for (i = 0; i < j; i++)
	{
		epsilon /= 10;
	}

	v_size = (int)(x_before->size);
	m_size = (int)(pow(v_size,2));
	ones = initialize_vector(v_size,1);
	d = initialize_vector(v_size,0);
	x_after = initialize_vector(v_size,0);

	P = initialize_graph(G,&d);

	// this section comes from expanding the equation P"= cP' + (1-c)E
	// expanding we get
	// P" = c(P + D) + (1-c)(ones x v^T)
	// P" = c(P +(d x v^T)) + (1-c)(ones x v^T)
	// P" = cP + c(d x v^T) + (1-c)(ones x v^T)
	// we can implement this as follows
	// P <- cP
	// P <- c(d x v^T) + P
	// P <- (1-c)(ones x v^T) + P
	// scale(&P,c);
	// alphaxtimesyTplusA(c,&d,v,&P);
	alphaxtimesyTplusA((1-c),&ones,v,&P);

	printf("P matrix in use:\n");
	print_DubMatrix(&P, v_size);

	do
	{			
		x_after = alphaATtimesx(c,&P,x_before);
		w = L1_difference(x_before,&x_after);		
		alphaxplusy_y(w,v,&x_after);		
		delta = differce_vector_length(&x_after,x_before);
		*x_before = makecopy(&x_after);

		*iter += 1;

		if (verbose[0] == 'y' || verbose[0] == 'Y' || verbose[0] == '\n')
		{
			printf("---------------------------------------------------------\n round %d\n",j++);
			printf("x to start round = \t");
			print_DubArray(x_before);
			printf("w = %6.4f\n", w );
			printf("x to end round = \t");
			print_DubArray(&x_after);
			printf("delta = %9.7f\n\n",delta );
		}

	}while(delta > epsilon);

	// printf("here\n");

	return x_after;


}



void obtain_graph_VE(char * filename, struct TwoDArray * a)
{
	
	int i,j,k, difference, max_col = 0;
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

			add_element(a,i-1,j);

			if (j > max_col)
			{
				max_col = j;
			}

		}
	}

	if (max_col > (a->size))
	{
		difference = max_col - (a->size);

		for(k= 0; k < difference; k++)
		{
			add_array(a);
		}
	}

	// sleep(1);
	fclose(ifp);
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