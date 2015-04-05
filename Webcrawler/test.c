#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * remove_extra(char * url)
{
    // printf("url = %s\n", url );

    char * result;

    // printf("here in remove extra\n");

    if (strchr(url,'\n') != NULL)
    {


        char * newurl = strstr(url,"//") + 2;
        int i = 0;

        // printf("url = %s\n", url);
        // printf("new_url = %s\n", newurl);

        result = (char *)malloc(strlen(newurl));

        while(newurl[i] != '\n')
        {
            result[i] = newurl[i];
            // printf("newurl[%d] = %c\n", i, newurl[i]);
            i++;
        }
        result[i] = '\0';

        // printf("result = %s.\n", result);
    }
    else
    {
        result = url;
    }


    return result;

}


int main (void)
{
	char test[] = "www.mcgill.ca/htsdadp/asdkmelme/asmdlmdsa?asdmmlaslm";

	printf("%s\n",test );

	printf("%s.\n",remove_extra(test));

	FILE * ifp = fopen("raw_links.txt","r");
	char * link;
	int i = 0;
	int bytes_read;
	size_t nbytes;

	while(i++ < 100)
	{
		
		bytes_read = getline(&link,&nbytes,ifp);
		printf("numbytes = %d, %s\n",bytes_read, remove_extra(link));
		// printf("here\n");
	}

	fclose(ifp);
}
