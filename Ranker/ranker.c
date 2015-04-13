#include <stdio.h>
#include <stdlib.h>

#define BUFFLEN 6000


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

int main()
{
	char * result = "/home/al/Documents/C/ECSE-518-project/PageRank/ranks.txt";
	char * links = "/home/al/Documents/C/ECSE-518-project/Webcrawler/filtered_links.txt";
	char * ranked_links = "ranked_links.txt";
	char link[BUFFLEN];
	char indexstring[10];
	int index;
	int index_found;

	char ch;
    int lines = 0;
    int i,j;
    i = j = 0;

	FILE * ifp_result = fopen(result,"r");
	if(ifp_result == NULL)
	{
		printf("Error in opening %s\n",result );
    	return(-1);
	}
	FILE * ifp_links = fopen(links,"r");
	if(ifp_links == NULL)
	{
		printf("Error in opening %s\n",links );
		fclose(ifp_result);
    	return(-1);
	}
	FILE * ofp = fopen(ranked_links,"w");
	if(ofp == NULL)
	{
		printf("Error in opening %s\n",ranked_links );
		fclose(ifp_result);
	    fclose(ifp_links);
    	return(-1);
	}

    //find the number of lines in the pagerank file
    while(!feof(ifp_links))
    {
      ch = fgetc(ifp_links);
      if(ch == '\n')
      {
        lines++;
      }
    }


    while(!feof(ifp_result))
    {
      ch = fgetc(ifp_result);
      if(ch == '\n')
      {
        lines--;
      }
    }


    if(lines != 0)
    {
    	printf("The two files did not contain the same number of links\nThe difference is %d\n", lines);
    	fclose(ifp_result);
	    fclose(ifp_links);
	    fclose(ofp);
    	return(-1);
    }
    else
    {
    	rewind(ifp_links);
    	rewind(ifp_result);
    }

    while(fgets(indexstring,10,ifp_result) != NULL)
    {
    	// this is the index of the next link that we want to print to the output file
    	index = string_to_int(indexstring);

    	// printf("index = %d\n", index);

    	if (index < j)
    	{
    		rewind(ifp_links);
    		j = 0;
    	}

    	for(i = j; i < index; i++)
    	{
    		if(fgets(link,BUFFLEN,ifp_links) == NULL)
    		{
    			printf("Unexpectedly at end of file\n");
    			return(-1);
    		}
    		j++;
    	}

    	fprintf(ofp, "%s", link);

    }

    fclose(ifp_result);
    fclose(ifp_links);
    fclose(ofp);

}
