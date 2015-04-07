#include <stdlib.h>
#include <stdio.h>
// #include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <cblas.h>

// struct DubArray
// {
//     double * array;
//     size_t size;
// };

// struct DubArray initialize_vector(int length, double value)
// {
//     // printf("here1\n");
//     struct DubArray temp;
//     int i;
//     if (length > 0)
//     {
//         temp.size = length;
//         temp.array = (double *)malloc(temp.size * sizeof(double));
//     }
//     else
//     {
//         temp.size = 0;
//         temp.array = (double *)malloc(sizeof(double));
//     }

//     for(i = 0; i < length; i++)
//     {
//         temp.array[i] = value;
//     }

//     return temp;
// }

int main()
{
    int row_size = 30;
    int matrix_elements = row_size*row_size;
    double * a = (double *)calloc(matrix_elements,sizeof(double));
    double *zeros = (double *)calloc(row_size,sizeof(double));
    int * bitmap = (int *)calloc(row_size,sizeof(int));
    int i, r = 0;
    srand(time(NULL));

    for(i = 0; i < 4; i++)
    {
        
        do
        {
            r += rand()%50;
            r = r % row_size;
            // printf("bitmap[%d] = %d\n",r, bitmap[r]);
            srand(time(NULL));
        }while(bitmap[r] != 0);

        bitmap[r] = 1;
        // printf("zeroing %dth row\n",r );
    }

    for (;i < matrix_elements; i++)
    {
        a[i] = i;
        // printf("%5.0f ",a[i]);
        // if(i%row_size == row_size-1)
        // {
        //     printf("\n");
        // }
    }
    // printf("\n\n");

    int j;

    double * to_zero;

    printf("here\n");
    for(i = 0; i < row_size; i++)
    {
        if (bitmap[i])
        {
            cblas_dcopy(row_size,zeros,1,&(a[i*row_size]),1);
            // printf("here with i = %d\n",i );
            // for(j = i*row_size; j < (i+1)*row_size; j++)
            // {
            //     a[j] = 0;
            // }
        }
    }
    printf("finished\n");

    for(i = 0; i < matrix_elements; i++)
    {
        printf("%4.0f ",a[i]);
        if(i%row_size == row_size-1)
        {
            printf("\n");
        }
    }

    printf("\n");


    free(a);
    free(bitmap);
    free(zeros);
}

/*------------------------------------------------------------------------------------------------------------------*/

// struct DubArray
// {
//     double * array;
//     size_t size;
// };

// int * taken;

// void print_dub(struct DubArray * a)
// {
//     int i;
//     for(i = 0; i < a->size; i++)
//     {
//         printf("%6.2f ", a->array[i]);
//     }
//     printf("\n");
// }

// void add_random_at_random(struct DubArray * a)
// {
//     int index;
//     double r = (double)(rand()%50)/1.2;
//     do
//     {
//         index = rand() % 20;
//     }while(taken[index]);

//     a->array[index] = r;
//     taken[index] = 1;
// }

// int main()
// {
//     srand(time(NULL));
//     struct DubArray test;
//     test.array = (double *)calloc(20,sizeof(double));
//     taken = (int *)calloc(20,sizeof(int));
//     test.size = 20;
//     int i = 0;
//     for(; i< 20; i++)
//     {
//         add_random_at_random(&test);
//         print_dub(&test);
//         printf("\n");
//     }

//     free(taken);
//     free(test.array);


//     return 0;
// }

/*------------------------------------------------------------------------------------------------------------------*/
// int main()
// {
// 	CURL *curl;
//     CURLcode curl_res;

//     curl_global_init(CURL_GLOBAL_ALL);

//     curl = curl_easy_init();

//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_URL, "http://bit.ly/1zKaphR");
//         curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
//     	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
//         // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)"); 

//         /* Perform the request, curl_res will get the return code */ 
//         curl_res = curl_easy_perform(curl);
//         // printf("here\n");

//         /* Check for errors */ 
//         if(curl_res != CURLE_OK)
//           fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                   curl_easy_strerror(curl_res));

//         if(CURLE_OK == curl_res) 
//         {
//              char *url;
//              curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
//              long code;

//              curl_res = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&code);
//              if((CURLE_OK == curl_res))
//                  printf("CURLINFO_RESPONSE_CODE: .%d.\n", (int)code);

//              if((CURLE_OK == curl_res) && url)
//                  printf("CURLINFO_EFFECTIVE_URL: .%s.\n", url);


//         }

//         /* always cleanup */ 
//         curl_easy_cleanup(curl);

//         /* we're done with libcurl, so clean it up */ 
//         curl_global_cleanup();

//     }
//     else
//     {
//         printf("cURL error.\n");
//     }

//     return 0;

// }