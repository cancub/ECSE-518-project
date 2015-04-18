#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    FILE * ifp = fopen(argv[1],"r");
    char str[2000];
    int i;

    while(fgets(str,2000,ifp) != NULL)
    {
        printf("\"");
        for(i = 0; i < strlen(str)-1; i++)
        {
            printf("%c", str[i]);
        }
        printf("\"\n");
        usleep(500000);
    }

    fclose(ifp);
    return 0;
}



// int hostnum = 0;
// int hostdesired = 20;
// char ** hostlist;

// char * switch_order_domain(char * url, char * domain)
// {

//     //domain = google.com

//     // http:(//www.google.com) or http:(//google.com)
//     char * beginning = strstr(url, "//");
//     char * middle, * end, * newurl, * testurl;

//     testurl = NULL;

//     if(beginning == NULL)
//     {
//         // if there isn't http(s):// for some reason
//         // if(testurl != NULL)
//         //     free(testurl);
//         // return NULL;
//         beginning = url;
//     }
//     else
//     {
//         // move past the double slash
//         beginning = &(beginning[2]);
//     }

//     // google.com/asdasd.asdsd
//     middle = strstr(beginning,domain);
//     end = strchr(middle,'.')+1;

//     // for strange links like www.alumni.mcgill.ca with a useless (www.)
//     if( (strchr(beginning,'.')+1 != middle) && (strstr(beginning,"www") == beginning) )
//     {
//         beginning = strchr(beginning,'.')+1;         //(www.alumni.mcgill.ca) --> www.(alumni.mcgill.ca)
//     }


//     if( (strchr(strchr(beginning,'.')+1,'.') == NULL) || 
//         ((strchr(strchr(beginning,'.')+1,'.') > strchr(beginning,'/')) &&
//         (strchr(strchr(beginning,'.')+1,'.') != NULL) && (strchr(beginning,'/') != NULL)))
//     {
//         // we have a link with only one period
//         // add a www to the beginning of beginning
//         testurl = (char *)malloc(strlen("www.") + strlen(beginning) + 1);     
//         testurl[4 + strlen(beginning)] = '\0';

//         strcpy(testurl,"www.");
//         strcpy(&(testurl[4]),beginning);
//         testurl[4 /* "www." */ + strlen(beginning)] = '\0';
//         beginning = testurl;
//     }

//     int final_length = (int)(strlen(beginning)) +1;
//     newurl = (char*)malloc(final_length);

//     // at this point, 
//     // beginning = <anything>.mcgill.ca/asdadddasd..asdsadasd
//     // middle = mcgill.ca/asdadddasd..asdsadasd
//     // end = ca/asdadddasd..asdsadasd

//     int endl,begl,midl;
//     char * seperator;


//     if(strchr(end,'/') != NULL)
//     {
//         seperator = strchr(end,'/');
//         endl = seperator - end;
//     }
//     else
//     {
//         seperator = strchr(end,'\0');
//         endl = seperator - end;
//     }

//     if(strchr(end,':') != NULL && strchr(end,':') < seperator)
//         endl -= (seperator - strchr(end,':'));

//     midl = end-middle-1;
//     begl = strstr(beginning,domain) - beginning - 1;

//     // printf("beginning = %s, length = %d\nmiddle = %s, length = %d\nend = %s, length = %d\n",beginning,begl,middle,midl,end,endl );

//     strncpy(newurl,end,endl);
//     newurl[endl] = '.';
//     strncpy(&(newurl[endl+1]), middle,midl);
//     newurl[midl+endl+1] = '.';
//     strncpy(&(newurl[midl+endl+2]),beginning,begl);
//     if(strchr(end,'/') != NULL)
//     {
//         newurl[midl+endl+begl + 2] = '/';
//         strcpy(&(newurl[midl+endl+begl + 3]),strchr(end,'/')+1);
//     }
//     else
//         newurl[midl+endl+begl + 2] = '\0';
    

//     if(testurl != NULL)
//         free(testurl);


//     return newurl;
// }

// char * host_check(char * host, char * filtered_hyperlink)
// {
//     //host = "cs", filtered_hyperlink = "ca.mcgill.caa.bc/asdadasd.aasd"

//     // caa.bc/asdadasd.aasd
//     char * soft_host = strchr(strchr(filtered_hyperlink,'.')+1,'.')+1;

//     int host_size;

//     if(strchr(soft_host,'/') != NULL)
//         host_size = strchr(soft_host,'/')-soft_host+1;
//     else
//         host_size = strchr(soft_host,'\0')-soft_host+1;

//     char * test_host = (char*)malloc(host_size+1);
//     memset(test_host,'\0', host_size+1);
//     strncpy(test_host,soft_host,host_size-1);

//     if(strcmp(host,test_host) != 0)
//     {
//         return test_host;
//     }
//     else
//     {
//         free(test_host);
//         return NULL;
//     }
// }

// int add_hostname(char * url, char * domain)
// {
//     char * host = switch_order_domain(url,domain);
//     char * new_host = host_check("",host);

//     int i;

//     for(i = 0; i < hostnum; i++)
//     {
//         if(strcmp(new_host,hostlist[i]) == 0)
//         {
//             //we have a match
//             free(host);
//             free(new_host);
//             return 1;
//         }
//     }

//     // if we make it to this point, this signifies that the host was not in the list

//     if(hostnum < hostdesired)
//     {
//         FILE * ifp = fopen("hostnames.txt","a");
//         // add the new host
//         hostlist[hostnum++] = new_host;
//         free(host);
//         fprintf(ifp, "%s\n", new_host);
//         fclose(ifp);
//         return 1;
//     }

//     free(host);
//     free(new_host);
//     return 0;
// }

// int main()
// {

//     hostlist = (char**)calloc(hostdesired,sizeof(char*));

//     char * url1 = "http://www.mcgill.ca";
//     char * url2 = "http://cs.mcgill.ca";
//     char * url3 = "http://cs.mcgill.ca/test/";

//     char * domain = "mcgill.ca";

//     int test = add_hostname(url1,domain);
//     test = add_hostname(url2,domain);
//     test = add_hostname(url3,domain);
//     printf("%d\n", test);


//     int i;

//     for(i = 0; i < hostnum; i++)
//     {
//         printf("host[%d] = %s\n",i,hostlist[i] );
//         free(hostlist[i]);
//     }

//     free(hostlist);

// }


// char * remove_extra(char * url)
// {
//     // printf("url = %s\n", url );

//     char * result;

//     // printf("here in remove extra\n");

//     if (strchr(url,'\n') != NULL)
//     {


//         char * newurl = strstr(url,"//") + 2;
//         int i = 0;

//         // printf("url = %s\n", url);
//         // printf("new_url = %s\n", newurl);

//         result = (char *)malloc(strlen(newurl));

//         while(newurl[i] != '\n')
//         {
//             result[i] = newurl[i];
//             // printf("newurl[%d] = %c\n", i, newurl[i]);
//             i++;
//         }
//         result[i] = '\0';

//         // printf("result = %s.\n", result);
//     }
//     else
//     {
//         result = url;
//     }


//     return result;

// }


// int main (void)
// {
// 	char test[] = "www.mcgill.ca/htsdadp/asdkmelme/asmdlmdsa?asdmmlaslm";

// 	printf("%s\n",test );

// 	printf("%s.\n",remove_extra(test));

// 	FILE * ifp = fopen("raw_links.txt","r");
// 	char * link;
// 	int i = 0;
// 	int bytes_read;
// 	size_t nbytes;

// 	while(i++ < 100)
// 	{
		
// 		bytes_read = getline(&link,&nbytes,ifp);
// 		printf("numbytes = %d, %s\n",bytes_read, remove_extra(link));
// 		// printf("here\n");
// 	}

// 	fclose(ifp);
// }
