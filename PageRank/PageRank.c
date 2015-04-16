#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "dubarrays.h"
#include "twodarrays.h"
#include "dubvmalg.h"

#define PERIODAD	1
#define PERIODDEL	100

// struct IntArray
// {
// 	int * array;
// 	size_t size;
// };


struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_0, struct DubArray * v, int * iter, double epsilon);
struct DubArray get_AdaptivePageRank(struct TwoDArray * G, struct DubArray * x_before, struct DubArray * v, int * iter, double epsilon);
void obtain_graph_VE(char * filename, struct TwoDArray * a);
void print_order(struct DubArray * result);
void mtranspose(struct DubArray * A, int n);
void add_directory(char ** str, int size, char * file);


double epsilon = 0.001;

int adaptive = 0;

int * converge;

int main (int argc, char *argv[]){
	
	char * filename = (char*)malloc(1024);
	struct TwoDArray temp_array;
	struct DubArray x_0, start_v, result;
	// char filenum;
	// char * x_type = (char*)malloc(sizeof(char));
	double x_vals,fraction;
	int i, itercount = 0;

	converge = (int*)calloc(2048, sizeof(int));

	construct_2DArray(&temp_array);

	// printf("Enter graph filename: ");
	// scanf("%s", filename);
	if (argc == 3)
	{
		
		filename = argv[1];
		adaptive = argv[2][0] - '0';

	}
	else if (argc == 2)
	{
		if(strlen(argv[1]) == 1)
		{
			adaptive = argv[1][0] - '0';
			add_directory(&filename,1024,"output.txt");
		}
		else
			filename = argv[1];
	}
	else
	{
		filename = "web-Google.txt";
	}
	// printf("Type of starting x <e>ven/<o>nes: ");
	// scanf("%s", x_type);
	// x_type = "e";

	obtain_graph_VE(filename,&temp_array);

	// print_2DArray(&temp_array);
	// sleep(1);

	fraction = 1/(double)(temp_array.size);

	if (0 /*x_type[1] == 'e'*/)
	{
		x_vals = 1;
	}
	else
	{
		x_vals = fraction;
	}

	// free(x_type);

	start_v = initialize_vector((int)(temp_array.size), fraction );
	x_0 = initialize_vector((int)(temp_array.size), x_vals);

	clock_t start = clock(), diff;
	if(adaptive)
	{
		printf("Using adaptive PageRank on graph in %s\n", filename);
		result = get_AdaptivePageRank(&temp_array, &x_0, &start_v, &itercount, epsilon);
	}
	else
	{	
		printf("Using pure PageRank on graph in %s\n", filename);
		result = get_PageRank(&temp_array, &x_0, &start_v, &itercount, epsilon);
	}
	diff = clock() - start;
	int msec = diff*1000/CLOCKS_PER_SEC;

	free(start_v.array);
	free(x_0.array);

	if(result.array == NULL)
	{
		printf("The PageRank vector could not converge.\n");
		return 0;
	}

	// printf("Result = \n");
	// print_DubArray(&result);
	print_order(&result);
	printf("\n");

	printf("Time to converge = %5.3f s\n", (double)(msec/1000)+((double)(msec%1000))/1000 );
	printf("Iterations to converge = %d\n", itercount);

	free(result.array);
	destruct_2DArray(&temp_array);

	int * histo = (int*)calloc(1000,sizeof(int));
	int max = 0;

	// printf("almost done\n");
	for(i = 0; i < temp_array.size; i++)
	{
		printf("converge[%d] = %d \n", i,converge[i]);
		histo[converge[i]] += 1;
		if (converge[i] > max)
			max = converge[i];
	}

	for(i = 0; i < max; i++)
		printf("histogram[%d] = %d\n",i,histo[i] );
	printf("\n");
	free(converge);
	free(histo);
	// free(filename);
	
	return 0;

}





















// functions



struct DubArray get_PageRank(struct TwoDArray * G, struct DubArray * x_before, struct DubArray * v, int * iter, double epsilon)
{	
	struct DubArray d,P,ones,x_after;
	int v_size,i; 
	double c;
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

	v_size = (int)(x_before->size);
	ones = initialize_vector(v_size,1);
	d = initialize_vector(v_size,0);
	x_after = initialize_vector(v_size,0);

	P = initialize_graph(G,&d);

	/* 
	this section comes from expanding the equation P"= cP' + (1-c)E
	expanding we get
	P" = c(P + D) + (1-c)(ones x v^T)
	P" = c(P +(d x v^T)) + (1-c)(ones x v^T)
	P" = cP + c(d x v^T) + (1-c)(ones x v^T)
	we can implement this as follows
	P <- cP
	P <- c(d x v^T) + P
	P <- (1-c)(ones x v^T) + P 
	*/
	scale(&P,c);
	alphaxtimesyTplusA(c,&d,v,&P);
	alphaxtimesyTplusA((1-c),&ones,v,&P);
	mtranspose(&P, v_size);		// P <- P^T


	printf("P matrix in use:\n");
	print_DubMatrix(&P, v_size);
	// printf("\n");

	i = 0;

	do
	{	

		free(x_after.array);
		x_after = alphaATtimesx(1,&P,x_before); 	// the result of an iteration of pure pagerank
		delta = differce_vector_length(&x_after,x_before);	// final difference between the initial and final vectors of this iteration

		*iter += 1;

		if (verbose[0] == 'y' || verbose[0] == 'Y' || verbose[0] == '\n')
		{
			printf("---------------------------------------------------------\n Round %d\n",i++);
			printf("x to start round = \t");
			print_DubArray(x_before);
			printf("x to end round = \t");
			print_DubArray(&x_after);
			printf("delta = %9.7f\n\n",delta );
			sleep(1);
		}
		// free(x_after);


		// move the values to start the next iteration
		free((*x_before).array);
		*x_before = makecopy(&x_after);


		// printf("pre_delta - delta = %10.8f\n", dub_abs(pre_delta - delta));
		// printf("epsilon - delta = %10.8f\n", epsilon - delta );

	}while(delta > epsilon);


	if (delta > epsilon)
	{
		x_after.array = NULL;
	}

	free(ones.array);
	free(d.array);
	free(P.array);

	return x_after;
}

struct DubArray get_AdaptivePageRank(struct TwoDArray * G, struct DubArray * x_before, struct DubArray * v, int * iter, double epsilon)
{	
	struct DubArray d,A_pp,ones,x_after,x_converged,x_test, A;
	int v_size,i, converged_count = 0; 
	double c;
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

	v_size = (int)(x_before->size);
	ones = initialize_vector(v_size,1);
	d = initialize_vector(v_size,0);
	x_after = initialize_vector(v_size,0);
	x_converged = initialize_vector(v_size,0);

	// C is the bitmap for elements which have converged or not
	// C[i] = 0 if the element has yet to converge
	// C[i] = 1 if the element has converged
	int * C = (int*)calloc(v_size,sizeof(int));

	A = initialize_graph(G,&d);

	/* 
	In keeping with the labeling in the paper, A will be our original pagerank matrix (the transpose of the P'' matrix)
	and A_pp will be A'', the matrix from which we shall periodically zero out rows
	*/
	scale(&A,c);
	alphaxtimesyTplusA(c,&d,v,&A);
	alphaxtimesyTplusA((1-c),&ones,v,&A);
	mtranspose(&A, v_size);			// A <- P^T, this will remain as the 
	A_pp = makecopy(&A);			// A is the matrix that will remain as is for periodic testing


	// printf("P matrix in use:\n");
	// print_DubMatrix(&P, v_size);
	// printf("\n");

	i = 0;

	do
	{	

		free(x_after.array);
		x_after = alphaATtimesx(1,&A_pp,x_before);		// x_after <- A''x_before
		alphaxplusy_y(1,&x_converged,&x_after);	// x_after <- A''x_before + x_converged
		
		

		if(((*iter) % PERIODDEL) == 0)
		{
			x_test = alphaATtimesx(1,&A,x_before);
			delta = differce_vector_length(&x_test,x_before);
			free(x_test.array);
		}

		if(((*iter) % PERIODAD) == 0)
		{
			// update the bitmap for convergence to determine if any elements have converged
			// and then update x_converged with a zero in that location and the PageRank matrix
			// with zeros in the column corresponding to those indices
			detect_converged(x_before,&x_after, &x_converged, epsilon, &C, &A_pp, &converged_count);

			if (converged_count == v_size)
			{
				printf("here\n");
				// print_DubArray(&x_after);
				break;
			}
			for(i = 0; i < v_size; i++)
			{
				if(C[i] == 1 && converge[i] == 0)
					converge[i] = *iter;
			}
			// printf("\n\n");
		}

		*iter += 1;

		if (verbose[0] == 'y' || verbose[0] == 'Y' || verbose[0] == '\n')
		{
			printf("---------------------------------------------------------\n Round %d\n",i++);
			printf("x to start round = \t");
			print_DubArray(x_before);
			printf("x to end round = \t");
			print_DubArray(&x_after);
			printf("delta = %9.7f\n\n",delta );
			sleep(1);
		}
		// free(x_after);

		// move the values to start the next iteration
		free((*x_before).array);
		*x_before = makecopy(&x_after);
		// printf("x_after bottom\n");
		// print_DubArray(&x_after);
		// printf("x_before bottom\n");
		// print_DubArray(x_before);
		// printf("\n\n");


		// printf("pre_delta - delta = %10.8f\n", dub_abs(pre_delta - delta));
		// printf("epsilon - delta = %10.8f\n", epsilon - delta );

	}while(delta > epsilon);

	// printf("here\n");

	if ((delta > epsilon) && (converged_count != v_size))
	{
		x_after.array = NULL;
	}

	free(ones.array);
	free(d.array);
	free(A.array);
	free(A_pp.array);
	free(x_converged.array);
	free(C);

	return x_after;
}


void print_order(struct DubArray * a)
{
	int i,j,k;
	double maxval = 0;
	int max_index = 0;

	FILE * ofp = fopen("ranks.txt","w");
	if (ofp == NULL)
	{
		printf("Error in opening ranks.txt\n");
		exit(0);
	}

	// we will use this to store the indices we've already printed
	int * printed = (int *)malloc((int)(a->size)*sizeof(int));

	// while there are still indices left to print
	for(k = 0; k < a->size; k++)
	{
		// find the next index for testing
		for (i = 0; i < a->size; i++)
		{

			// cycle through the array of printed indices to see if this
			// one has already been printed
			for(j = 0; j < k; j++)
			{
				// printf("i = %d, j = %d,k = %d, printed[%d] = %d\n",i,j,k,j,printed[j] );
				if(printed[j] == i)
				{
					break;
				}
			}

			// printf("a->array[%d] = %f, maxval = %f, max_index = %d \n", i,a->array[i],maxval,max_index);
			if((a->array[i] > maxval) && (j == k))
			{
				maxval = a->array[i];
				max_index = i;
			}

			// usleep(6000);
		}		

		printed[k] = max_index;
		// printf("%d\n",max_index+1);
		fprintf(ofp, "%d\n",max_index+1 );

		maxval = 0;
	}

	free(printed);

	fclose(ofp);

	// printf("\n");

}

void obtain_graph_VE(char * filename, struct TwoDArray * a)
{
	
	int i,j,k, difference, max_col = 0;
	// struct Array current_vertex;
	// open the file containing the graph
	FILE * ifp = fopen(filename,"r");
	if (ifp == NULL)
	{
		// let the caller know an error occured
		fprintf(stderr, "Cannot open file %s\n", filename);
	}
	else
	{
		// printf("here1\n");
		// we have successfully opened the file
		while(fscanf(ifp, "%d %d", &i, &j) == 2)
		{
			// there should be two values listed in this directed graph on each line
			// i = from
			// j = to
			if(i < 60000 && j < 60000)
			{
				// printf("i = %d, j = %d, array size = %d\n",i,j, (int)(a->size));

				// we need to add arrays for both this new node and the nodes
				// that we may have skipped (to make a complete vector)
				if (i > (a->size))
				{
					difference = i - (a->size);

					for(k= 0; k < difference; k++)
					{
						// printf("k = %d, difference = %d\n",k,difference );
						add_array(a);
					}
				}

				add_element(a,i-1,j);
				// printf("current size = %d\n",(int)((a->array[i-1]).size));
				// printf("\n\n");
				// sleep(1);

				if (j > max_col)
				{
					max_col = j;
				}
			}

			// print_2DArray(a);

			// sleep(1);

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

	// print_2DArray(a);
	// exit(1);

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

void add_directory(char ** str, int size, char * file)
{
	getcwd(*str,size);

	char * test = *str;

	while(strchr(test,'/') != NULL)
	{
		test = strchr(test,'/')+1;
	}

	strcpy(&((*str)[test-*str]),file);

}

