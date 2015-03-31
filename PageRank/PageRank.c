#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "dubarrays.h"
#include "twodarrays.h"
#include "dubvmalg.h"

struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_0, struct DubArray * v, int * iter);
void obtain_graph_VE(char * filename, struct TwoDArray * a);
double dub_abs(double a);


int main (){
	
	char * filename;
	struct TwoDArray temp_array;
	struct DubArray x_0, start_v, result, failed;
	// char filenum;
	char * x_type = (char*)malloc(sizeof(char));
	double x_vals,fraction;
	int itercount = 0;

	construct_2DArray(&temp_array);

	// printf("Enter graph filename: ");
	// scanf("%s", filename);
	filename = "web-Google.txt";
	// printf("Type of starting x <e>ven/<o>nes: ");
	// scanf("%s", x_type);
	x_type = "e";

	obtain_graph_VE(filename,&temp_array);

	fraction = 1/(double)(temp_array.size);

	if (x_type[1] == 'e')
	{
		x_vals = 1;
	}
	else
	{
		x_vals = fraction;
	}

	start_v = initialize_vector((int)(temp_array.size), fraction );
	x_0 = initialize_vector((int)(temp_array.size), x_vals);
	failed = initialize_vector((int)(temp_array.size), 0);

	clock_t start = clock(), diff;
	result = get_PageRank(&temp_array, &x_0, &start_v, &itercount);
	diff = clock() - start;
	int msec = diff*1000/CLOCKS_PER_SEC;

	if(compare_DubArrays(&result,&failed))
	{
		printf("The PageRank vector could not converge.\n");
		return 0;
	}
	printf("Result = ");
	print_DubArray(&result);
	printf("\n");

	printf("Time to converge = %5.3f s\n", (double)(msec/1000)+((double)(msec%1000))/1000 );
	printf("Iterations to converge = %d\n", itercount);

	return 0;

}







// functions


struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_before, struct DubArray * v, int * iter)
{	
	struct DubArray d,P,ones,x_after;
	int v_size,i,j = 0; 
	double w, pre_delta, c, epsilon = 1;
	double  delta = 0;
	char * verbose;

	// printf("Input damping factor c: ");
	// scanf("%lf", &c);
	c=0.85;
	// printf("Test value epsilon = 1x10^-");
	// scanf("%d", &j);
	// printf("Verbose? <y/n>: ");
	// scanf("%s", verbose);
	verbose = "n";

	// for (i = 0; i < j; i++)
	// {
	// 	epsilon /= 10;
	// }
	epsilon = 0.0001;

	j = 0;

	v_size = (int)(x_before->size);
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
	scale(&P,c);
	alphaxtimesyTplusA(c,&d,v,&P);
	alphaxtimesyTplusA((1-c),&ones,v,&P);

	// printf("P matrix in use:\n");
	// print_DubMatrix(&P, v_size);
	// printf("\n");

	i = 0;

	do
	{	
		pre_delta = delta;		
		x_after = alphaATtimesx(c,&P,x_before);
		w = L1_difference(x_before,&x_after);		
		alphaxplusy_y(w,v,&x_after);		
		delta = differce_vector_length(&x_after,x_before);
		*x_before = makecopy(&x_after);

		*iter += 1;

		printf("Round %d\n", i++);

		if (verbose[0] == 'y' || verbose[0] == 'Y' || verbose[0] == '\n')
		{
			printf("---------------------------------------------------------\n round %d\n",j++);
			printf("x to start round = \t");
			print_DubArray(x_before);
			printf("w = %6.4f\n", w );
			printf("x to end round = \t");
			print_DubArray(&x_after);
			printf("delta = %9.7f\n\n",delta );
			sleep(1);
		}

		// printf("pre_delta - delta = %10.8f\n", dub_abs(pre_delta - delta));
		// printf("epsilon - delta = %10.8f\n", epsilon - delta );

	}while((delta > epsilon) && (dub_abs(pre_delta - delta) > epsilon/10000));

	// printf("here\n");

	if (delta > epsilon)
	{
		x_after = initialize_vector(v_size,0);
	}

	return x_after;


}

double dub_abs(double a)
{
	if (a < 0)
	{
		a *= -1;
	}

	return a;
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
		printf("here1\n");
		while(fscanf(ifp, "%d %d", &i, &j) == 2)
		{
			printf("i = %d, j = %d\n",i,j);

			if (i > (a->size))
			{
				difference = i - (a->size);

				for(k= 0; k < difference; k++)
				{
					printf("k = %d, difference = %d\n",k,difference );
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