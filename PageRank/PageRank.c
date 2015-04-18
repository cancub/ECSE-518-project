/*
To run this program, type the following into the terminal:
./pagerank <location of file containing web graph if not in this folder>

example of a valid call for files in this fodler structure

./pagerank ../Webcrawler/1000output.txt

0 = pure pagerank
1 = APR
2 = MAPR

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "dubarrays.h"
#include "twodarrays.h"
#include "dubvmalg.h"

#define PERIODAD	8
#define PERIODDEL	5

// struct IntArray
// {
// 	int * array;
// 	size_t size;
// };


struct DubArray initialize_graph(struct TwoDArray * a, struct DubArray * no_out);
struct DubArray PageRank(struct DubArray * A, struct DubArray * d, struct DubArray * x_0, struct DubArray * v, 
	int * iter, double epsilon);
struct DubArray filterAPR(struct DubArray * A, struct DubArray * d, struct DubArray * x_before, struct DubArray * v,
	int * iter, double epsilon);
struct DubArray filterMAPR(struct DubArray * A, struct DubArray * d, struct DubArray * x_before, struct DubArray * v, 
	int * iter, double epsilon);
void obtain_graph_VE(char * filename, struct TwoDArray * a);
void print_order(struct DubArray * result);
char * add_directory(char ** str, int size, char * file);
int string_to_int(char * test);


double epsilon;

int adaptive;

// int * converge;

int main (int argc, char *argv[])
{	
	char * filename = (char*)malloc(1024);
	char * new_filename;
	struct TwoDArray temp_array;
	struct DubArray x_0, start_v, result, A,d, ones;
	// char filenum;
	// char * x_type = (char*)malloc(sizeof(char));
	double x_vals,fraction;
	int i, itercount = 0;
	int * histo;
	int max = 0;
	int remove_filename = 0;
	double c = 0.85;

	epsilon = 0.001;
	adaptive = 0;

	construct_2DArray(&temp_array);

	// printf("Enter graph filename: ");
	// scanf("%s", new_filename);
	if (argc == 3)
	{
		
		new_filename = argv[1];
		adaptive = argv[2][0] - '0';

	}
	else if (argc == 2)
	{
		if(strlen(argv[1]) == 1)
		{
			adaptive = argv[1][0] - '0';
			new_filename = add_directory(&filename,1024,"output.txt");
			remove_filename = 1;
		}
		else
			new_filename = argv[1];
	}
	else
	{
		new_filename = "web-Google.txt";
	}
	// printf("Type of starting x <e>ven/<o>nes: ");
	// scanf("%s", x_type);
	// x_type = "e";

	obtain_graph_VE(new_filename,&temp_array);

	d = initialize_vector(temp_array.size,0);
	A = initialize_graph(&temp_array,&d);	
	

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
	// histo = (int*)calloc(2048,sizeof(int));
	// converge = (int*)calloc((int)(temp_array.size),sizeof(int));
	ones = initialize_vector((int)(temp_array.size),1);
	start_v = initialize_vector((int)(temp_array.size), fraction );
	x_0 = initialize_vector((int)(temp_array.size), x_vals);

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

	printf("Obtaining initial graph matrix\n");
	scale(&A,c);
	alphaxtimesyTplusA(c,&d,&start_v,&A);
	alphaxtimesyTplusA((1-c),&ones,&start_v,&A);
	mtranspose(&A, (int)(temp_array.size));		// P <- P^T
	printf("\"A\" matrix created\n");

	free(ones.array);
	free(d.array);

	clock_t start, diff;
	int msec;
	char * to_run;

	while(1)
	{
		printf("0: Pure PageRank\n1: APR\n2: MAPR\nx: quit\n");

		if(scanf("%s",to_run) < 0)
		{
			printf("Some sort of error, try again\n");
			continue;
		}

		if((strcmp(to_run,"0") != 0) && (strcmp(to_run,"1") != 0)  && (strcmp(to_run,"2") != 0))
			break;

		itercount = 0;
		adaptive = string_to_int(to_run);

		start_v = initialize_vector((int)(temp_array.size), fraction );
		x_0 = initialize_vector((int)(temp_array.size), x_vals);

		start = clock();
		if(adaptive == 1)
		{
			printf("Using adaptive PageRank on graph in %s\n", new_filename);
			result = filterAPR(&A, &d, &x_0, &start_v, &itercount, epsilon);
		}
		else if (adaptive == 2)
		{	
			printf("Using modified adaptive PageRank on graph in %s\n", new_filename);
			result = filterMAPR(&A,&d, &x_0, &start_v, &itercount, epsilon);	
		}
		else
		{	
			printf("Using pure PageRank on graph in %s\n", new_filename);
			result = PageRank(&A,&d, &x_0, &start_v, &itercount, epsilon);		
		}
		diff = clock() - start;
		msec = diff*1000/CLOCKS_PER_SEC;


		if(result.array == NULL)
		{
			printf("The PageRank vector could not converge.\n");
			return 0;
		}
		else
		{	
			printf("Algorithm complete\n");
		}
	

		print_order(&result);

		printf("Time to converge = %5.3f s\n", (double)(msec/1000)+((double)(msec%1000))/1000 );
		printf("Iterations to converge = %d\n", itercount);

		free(result.array);
		free(start_v.array);
		free(x_0.array);
		
	}

	destruct_2DArray(&temp_array);
	// // printf("almost done\n");
	// for(i = 0; i < temp_array.size; i++)
	// {
	// 	// printf("converge[%d] = %d \n", i,converge[i]);
	// 	histo[converge[i]] += 1;
	// 	if (converge[i] > max)
	// 	{
	// 		max = converge[i];
	// 	}
	// }

	// FILE * conv = fopen("convegence_histo.txt","w");
	// for(i = 0; i < max; i++)
	// {
	// 	fprintf(conv,"histogram[%d] = %d\n",i,histo[i] );
	// }
	// fclose(conv);
	// printf("\n");

	// free(converge);
	if (remove_filename)
		free(new_filename);
	free(filename);
	// free(histo);
	free(A.array);
	// free(filename);
	
	return 0;

}





















// functions



struct DubArray PageRank(struct DubArray * A, struct DubArray * d, struct DubArray * x_before, struct DubArray * v, 
	int * iter, double epsilon)
{	
	struct DubArray x_after;
	int i, v_size; 
	double  delta = 1;
	char * verbose;

	// printf("Input damping factor c: ");
	// scanf("%lf", &c);
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
	x_after = initialize_vector(v_size,0);

	// printf("P matrix in use:\n");
	// print_DubMatrix(&P);
	// printf("\n");

	i = 0;

	printf("Beginning PageRank algorithm\n");

	do
	{	
		*iter += 1;

		free(x_after.array);
		x_after = alphaAtimesx(A,x_before); 	// the result of an iteration of pure pagerank
		delta = differce_vector_length(&x_after,x_before);	// final difference between the initial and final vectors of this iteration

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

		printf("Iteration %d complete, delta = %10.8f\n", *iter, delta );

	}while(delta > epsilon);


	if (delta > epsilon)
	{
		x_after.array = NULL;
	}

	return x_after;
}

struct DubArray filterAPR(struct DubArray * A, struct DubArray * d, struct DubArray * x_before, struct DubArray * v, int * iter, double epsilon)
{	
	struct DubArray A_pp,x_after,x_converged,x_test, zeros;
	int v_size,i, converged_count = 0; 
	double  delta = 1;
	char * verbose;

	// printf("Input damping factor c: ");
	// scanf("%lf", &c);
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
	x_after = initialize_vector(v_size,0);
	x_converged = initialize_vector(v_size,0);
	zeros = initialize_vector(v_size,0);

	// C is the bitmap for elements which have converged or not
	// C[i] = 0 if the element has yet to converge
	// C[i] = 1 if the element has converged
	int * C = (int*)calloc(v_size,sizeof(int));

	/* 
	In keeping with the labeling in the paper, A will be our original pagerank matrix (the transpose of the P'' matrix)
	and A_pp will be A'', the matrix from which we shall periodically zero out rows
	*/
	printf("Obtaining A''\n");
	A_pp = makecopy(A);			// A is the matrix that will remain as is for periodic testing

	printf("A'' obtained\n");

	// printf("P matrix in use:\n");
	// print_DubMatrix(&P);
	// printf("\n");

	i = 0;

	printf("Beginning Adaptive PageRank Algorithm\n");

	do
	{	
		free(x_after.array);
		x_after = A_times_x_plus_y(&A_pp,x_before,&x_converged);		

		*iter += 1;
		

		if(((*iter) % PERIODAD) == 0)
		{
			// update the bitmap for convergence to determine if any elements have converged
			// and then update x_converged with a zero in that location and the PageRank matrix
			// with zeros in the column corresponding to those indices
			detect_converged(x_before,&x_after, &x_converged, epsilon, &C, &A_pp, &converged_count);

			if (converged_count == v_size)
			{
				// all elements have converged
				// print_DubArray(&x_after);
				printf("Iteration %d complete, total element-wise convergence\n", *iter );
				break;
			}
			// for(i = 0; i < v_size; i++)
			// {
			// 	if(C[i] == 1 && converge[i] == 0)
			// 		converge[i] = *iter;
			// }
			// printf("\n\n");
		}

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

		if(((*iter) % PERIODDEL) == 0)
		{
			x_test = alphaAtimesx(A,x_before);
			delta = differce_vector_length(&x_test,x_before);
			free(x_test.array);
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

		printf("Iteration %d complete, delta = %10.8f\n", *iter, delta );

	}while(delta > epsilon);

	if ((delta > epsilon) && (converged_count != v_size))
	{
		x_after.array = NULL;
	}

	free(A_pp.array);
	free(x_converged.array);
	free(C);
	free(zeros.array);

	return x_after;
}

struct DubArray filterMAPR(struct DubArray * A,  struct DubArray * d,struct DubArray * x_before, struct DubArray * v, int * iter, double epsilon)
{	
	struct DubArray A_NN, A_CN,x_after,x_converged,x_test,y, zeros;
	int v_size,i, converged_count = 0; 
	double  delta = 1;
	char * verbose;

	// printf("Input damping factor c: ");
	// scanf("%lf", &c);
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
	x_after = initialize_vector(v_size,0);
	x_converged = initialize_vector(v_size,0);
	y = initialize_vector(v_size,0);
	zeros = initialize_vector(v_size,0);

	// C is the bitmap for elements which have converged or not
	// C[i] = 0 if the element has yet to converge
	// C[i] = 1 if the element has converged
	int * C = (int*)calloc(v_size,sizeof(int));

	printf("Obtaining A_NN matrix\n");

	A_NN = makecopy(A);				// this is the matrix for non-converged to non-converged edges, it will lose rows and 

	printf("A_NN obtained\nObtaining A_CN\n");
	A_CN = initialize_vector(v_size*v_size,0);	

	printf("A_CN obtained\n");

	// printf("P matrix in use:\n");
	// print_DubMatrix(&P);
	// printf("\n");

	i = 0;

	printf("Beginning Modified Adaptive PageRank Algorithm\n");

	do
	{	
		free(x_after.array);
		// x^(k+1) = A_NN * x^(k) + x''_C + y
		x_after = A_times_x_plus_y(&A_NN,x_before,&x_converged);
		alphaxplusy_y(1,&y,&x_after);
		

		*iter += 1;
		

		if(((*iter) % PERIODAD) == 0)
		{
			// update the bitmap for convergence to determine if any elements have converged
			// and then update x_converged with a zero in that location and the PageRank matrix
			// with zeros in the column corresponding to those indices

			// x_before and x_after are used to determine element-wise convergence
			// x_converged has elements added to it from x_after if that element has converged
			// epsilon is used as the threshold for element-wise convergence
			// C is a bitmap for converged elements
			// A_NN is the matrix containing elements corresponding to edges from non-converged to non-converged links
				// if the function determines that a certain link's pagerank has converged, the row AND column
				// corresponding to that element number are zeroed
			// A_CN is the matrix containing elements corresponding to edges between converged and non-converged links
				// if the function determines that a certain link's pagerank has converged, then that column
				// from A is added and all the rows for which C[element] = 0 are zeroed
			detect_convergedMAPR(x_before, &x_after, &x_converged, epsilon, &C, A, &A_NN, &A_CN, &converged_count);

			if (converged_count == v_size)
			{
				// all elements have converged
				printf("Iteration %d complete, total element-wise convergence\n", *iter );
				break;
			}
			// for(i = 0; i < v_size; i++)
			// {
			// 	if(C[i] == 1 && converge[i] == 0)
			// 		converge[i] = *iter;
			// }

			free(y.array);
			y = alphaAtimesx(&A_CN,x_before);

			// printf("Converged = \n");
			// for(i = 0; i < v_size; i++)
			// {
			// 	printf("%d", C[i]);
			// }
			// printf("\nA_NN = \n");
			// print_DubMatrix(&A_NN);
			// printf("A_CN = \n");
			// print_DubMatrix(&A_CN);
			// sleep(5);
		}

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

		if(((*iter) % PERIODDEL) == 0)
		{
			x_test = alphaAtimesx(A,x_before);
			delta = differce_vector_length(&x_test,x_before);
			// printf("delta = %7.6f\n",delta );
			// sleep(1);
			free(x_test.array);
		}

		// move the values to start the next iteration
		free((*x_before).array);
		*x_before = makecopy(&x_after);

		printf("Iteration %d complete, delta = %10.8f\n", *iter, delta );

	}while(delta > epsilon);

	if ((delta > epsilon) && (converged_count != v_size))
	{
		x_after.array = NULL;
	}

	free(A_NN.array);
	free(A_CN.array);
	free(x_converged.array);
	free(C);
	free(zeros.array);
	free(y.array);

	return x_after;
}

void print_order(struct DubArray * a)
{
	int i,k;
	double maxval = 0;
	int max_index = 0;
	char * location = (char*)malloc(2048);
	getcwd(location,2048);

	FILE * ofp = fopen("ranks.txt","w");
	if (ofp == NULL)
	{
		printf("Error in opening ranks.txt\n");
		exit(0);
	}
	else
	{
		printf("Printing ranked indices to file %s/ranks.txt\n", location);
	}

	// a bitmap indicating if the element at a certain index has already been printed
	int * printed = (int *)calloc((int)(a->size),sizeof(int));

	// while there are still indices left to print
	for(k = 0; k < a->size; k++)
	{
		// find the next index for testing
		for (i = 0; i < a->size; i++)
		{
			// printf("array[%d] = %5.4f, printed[%d] = %d\n", i, a->array[i], i, printed[i]);
			// save the details of this index if it is the biggest so far and has yet to be printed
			if((a->array[i] > maxval) && (printed[i] == 0))
			{
				maxval = a->array[i];
				max_index = i;
				// printf("maxval = %5.4f, max_index = %d\n", maxval, max_index);
			}
		}		

		// printf("printing to file\n");
		printed[max_index] = 1;
		fprintf(ofp, "%d\n",max_index+1 );

		maxval = 0;
	}

	free(printed);
	free(location);

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

char * add_directory(char ** str, int size, char * file)
{
	char * result = (char*)calloc(size,sizeof(char));
	memcpy(result,*str,size);
	getcwd(result,size);

	char * test = result;

	while(strchr(test,'/') != NULL)
	{
		test = strchr(test,'/')+1;
	}

	strcpy(&((result)[test-result]),file);

	return result;

}

int string_to_int(char * test)
{
    int tens = 1;
    int i = 0;
    int result = 0;

    while((test[i] != '\0') && (test[i] != '\n'))
    {
        i++;
    }

    for(i--;i >= 0; i--)
    {
        // printf("%c\n",test[i] );
        result += (test[i] - '0') * tens;
        tens *= 10;
    }

    return result;
}

