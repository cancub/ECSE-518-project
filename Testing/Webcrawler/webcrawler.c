// credit to user Jay Taylor for the wget call used here to retreive hyperlinks from a webpage
// http://stackoverflow.com/a/2804721

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "linked_list.h"
#include <curl/curl.h>

#define BUFFLEN         2500
#define COMPLETED       1
#define NOT_COMPLETED   0
#define STARTLINKS      1
#define WAITTIME        1000
#define RETRIES         3

struct stringArray
{
    char ** array;
    size_t size;
};

// NOTE THAT THE FORMAT FOR FILTERED LINKS IS 
// (completely filtered, extra removed, index of page that pointed to this one)

// WHILE THE FULLY PARSED LINKS ARE STORED AS
// (completely filtered, extra removed, this page's index)

//http://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

char * flip_url(char * url);
char * add_slash(char * url);
void * wget_wrapper(void * arg);
void crawl_page(char * inputfile, char * newinputfile, char * parsed_links, char * raw_links);
char * remove_extra(char * url);
void filter_and_store(char * raw, struct Linked_list * a);
void add_edge(struct Linked_list * a, int from_index, int to_index);
void add_link(struct Linked_list * a, char * link, int index);
char * clean_link(char * url);
int search_for_link(struct Linked_list * a, char * filtered_link);
void print_string(char * string_thing);
char * rm_invalid(char * url);
void finish_up(struct stringArray * parsed, FILE * ofp, char * raw_links, char * filtered_links);
void read_and_save(char * raw_links, FILE * ofp, struct stringArray * parsed_links, char * filtered_links, int add_links);
int quick_search(struct stringArray * parsed_links, char * link);
char * remove_slash(char ** link);
void print_links(struct stringArray * list);
char * redirected(char * original);
char * switch_order(char * url);


int index_under_wget = 1;
char raw_links[] = "raw_links.txt";

int thread_test;

int main(int argc, char *argv[])
{
    char * link, *starter, *newurl; 
    struct stringArray parsed_links;
    pthread_t wget_main;
    int count, retry;
    // struct node * temp;
    FILE * ofp = fopen("output.txt","w");

    char * filtered = "filtered_links.txt";

    if(ofp == NULL)
    {
        printf("Could not open output.txt for writing\n");
        return(-1);
    }

    if(argc == 2)
    {
        starter = argv[1];
    }
    else
    {
        starter = "http://www.google.com";    // the starting node
    }

    FILE * clean = fopen(filtered,"w");
    fprintf(clean, "%s\n",starter );
    fclose(clean);

    //initialize linked lists
    // parsed_links = initialize_linked_list(1);
    parsed_links.size = 1;
    parsed_links.array = (char **)malloc(sizeof(char *) * 1000);
    parsed_links.array[0] = remove_extra(starter);

    // free(starter);

    link = starter;

    // add_link(parsed_links,link,index_under_wget);

    do
    {        
        printf("\n-----------------------\n\nRound %d of %d:\n", index_under_wget,(int)(parsed_links.size));
        printf("looking at link %s\n",link);
        newurl = switch_order(link);
        printf("reversal of this link is %s\n",newurl);
        free(newurl);

        retry = 0;

        while(retry < RETRIES)
        {

            count = 0;

            thread_test = NOT_COMPLETED;
            pthread_create(&wget_main,NULL,wget_wrapper,link);

            while((thread_test == NOT_COMPLETED) && (count < (WAITTIME*(retry+1))))
            {
                usleep(100000);
                // printf("%d ",count );
                count++;
            }

            // printf("\n");

            if (count == WAITTIME)
            {
                pthread_cancel(wget_main);
                if(retry < RETRIES)
                    printf("Retrying...%d\n", retry);
                else
                    printf("Failed to retreive links\n");
                retry++;
            }
            else
            {
                pthread_join(wget_main,NULL);
                break;
            }
        }

        if (retry == RETRIES)
        {
            FILE * to_clear = fopen(raw_links,"w");
            fclose(to_clear);
        }



        // pthread_join(wget_main,NULL);

        // pthread_join(wget_main,NULL);
        // wget_wrapper(link, raw_links);

        // printf("here in main\n");

        read_and_save(raw_links,ofp, &parsed_links,filtered, 1);

        // printf("ici\n");

        // filter_and_store(raw_links, parsed_links);

        index_under_wget++;

        link = parsed_links.array[index_under_wget-1];

        // link = (get_node_at_index(parsed_links,index_under_wget-1))->hyperlink;

        // print_linked_list(parsed_links);

    }while(index_under_wget <= STARTLINKS);

    // sleep(5);

    finish_up(&parsed_links, ofp, raw_links, filtered);

    fclose(ofp);

    int j = 0;

    // printf("here\n");
    for (;j < parsed_links.size; j++)
    {
        free(parsed_links.array[j]);
    }
    free(parsed_links.array);


    /* we're done with libcurl, so clean it up */ 
    curl_global_cleanup();

    return 0;

}








//
//functions

char * redirected(char * original)
{
    CURL *curl;
    CURLcode curl_res;
    char * result = NULL;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, original);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)"); 

        /* Perform the request, curl_res will get the return code */ 
        curl_res = curl_easy_perform(curl);
        // printf("here\n");

        /* Check for errors */ 
        if(curl_res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(curl_res));

        if(CURLE_OK == curl_res) 
        {
            char *url;
            curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

            if((CURLE_OK == curl_res) && url)
            {
                // curl_res = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,code);
                if(strcmp(url,original) != 0)
                {
                    result = (char *)malloc(strlen(url) + 1);

                    strcpy(result,url);
                }
                // else
                // {   
                //     // printf("No redirect, using ...%s...\n",newlink);
                // }
            }
        }

    }
    else
    {
        printf("cURL error.\n");
    }
    

    /* always cleanup */ 
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return result;

}


void read_and_save(char * raw_links, FILE * ofp, struct stringArray * parsed_links, char * filtered_links, int add_links)
{
    // open the raw links file from wget
    
    char * link = NULL;
    char * newlink = NULL;
    int to_index, from_index = index_under_wget;
    size_t nbytes;
    int i;

    FILE * ifp = fopen(raw_links,"r");
    if(ifp == NULL)
    {
        printf("Could not open %s for reading\n", raw_links);
        return;
    }
    FILE * filtered = fopen(filtered_links,"a");
    if(ofp == NULL)
    {
        printf("Could not open %s for writing\n", filtered_links);
        return;
    }
    

    char ch;
    int lines = 0;
    int elements = 0;
    int added = 0;

    while(!feof(ifp))
    {
      ch = fgetc(ifp);
      if(ch == '\n')
      {
        lines++;
      }
    }

    printf("Number of links found = %d\n", lines);
    // sleep(1);

    // printf("beginning:\n");
    // print_links(parsed_links);
    // printf("\n");
    // // printf("to_index = %d\n",to_index );
    // printf("\n");
    // sleep(3);

    if(add_links)
    {
        printf("Adding edges and appending links\n");
    }
    else
    {
        printf("Adding edges\n");
    }

    if(lines)
    {
        rewind(ifp);

        int * possible_array = (int *)malloc(sizeof(int)*lines);

        char * redirected_link = NULL;

        char * test_link = NULL;

    
        while(getline(&link,&nbytes,ifp) != -1)
        {
            // printf("\n\n--------------\n");

            // for(i = 0; i < parsed_links->size; i++)
            // {
            //     printf("link %d = %s\n",i+1,(parsed_links->array)[i] );
            // }
            // printf("\n");

            // printf("here in read and save\n");

            free(newlink);
            free(test_link);
            newlink = NULL;
            test_link = remove_extra(link);
            // free(link);

            // printf("testing %s\n", link);


            if((strstr(test_link,"//goo.gl") != NULL) || (strstr(test_link,"//tinyurl") != NULL) ||
                (strstr(test_link,"//t.co") != NULL) || (strstr(test_link,"//x.co") != NULL) ||
                (strstr(test_link,"//is.gd") != NULL) || (strstr(test_link,"//bit.ly") != NULL) ||
                (strstr(test_link,"//owl.ly") != NULL) || (strstr(test_link,"//deck.ly") != NULL) ||
                (strstr(test_link,"//su.pr") != NULL) || (strstr(test_link,"//ink.co") != NULL))
            {
                printf("Testing for redirect: %s\n",test_link);

                redirected_link = redirected(test_link);

                if(redirected_link != NULL)
                {
                    newlink = remove_extra(redirected_link);
                    free(redirected_link);
                    printf("Changing to %s\n",newlink);
                    // sleep();
                }
                else
                {
                    printf("Not redirected\n");
                }
            }

            if (newlink == NULL)                              
                newlink = remove_extra(link);
            

            // printf("link from file = %s\n",newlink);

            to_index = quick_search(parsed_links,newlink);
           

            



            if(to_index != 0)
            {
                //we have an existing link

                for (i = 0; i < elements; i++)
                {
                    if(to_index == possible_array[i])
                        break;
                }

                if (i == elements)
                {
                    fprintf(ofp, "%d\t%d\n",from_index,to_index );

                    // printf("printing to file edge to existing node : \t%d----->%d\n", from_index,to_index);
                    possible_array[elements] = to_index;
                    elements++;
                }
                else
                {
                    // printf("There is already an edge between these nodes:\t%d----->%d\n", from_index,to_index);
                }
            }
            else
            {
                //only add the link to the newly discovered node if we are in the initial stages of
                // link apprehension
                
                if (add_links)
                {
                    parsed_links->size += 1;
                    to_index = parsed_links->size;
                    parsed_links->array = (char **)realloc(parsed_links->array, sizeof(char *) * (parsed_links->size));
                    // printf("Adding %s to index %d. String size = %d\n",newlink, to_index,(int)(strlen(newlink)) );
                    (parsed_links->array)[to_index - 1] = (char *)malloc((int)(strlen(newlink))+2);
                    if((parsed_links->array)[to_index - 1] == NULL)
                    {
                        printf("Error in allocating memory for links\n");
                    }
                    memcpy((parsed_links->array)[to_index - 1], newlink, strlen(newlink)+1);
                    fprintf(ofp, "%d\t%d\n",from_index,to_index );
                    fprintf(filtered, "%s\n", newlink);   

                    // printf("printing to file new edge: \t%d----->%d\n", from_index,to_index);

                    // if(strstr(newlink,"aoc.mcgill.ca/site") != NULL)
                    // {
                    //     printf("\n\n");
                    //     printf("link = %s, to_index = %d\n", newlink,to_index);
                    //     printf("array[to_index - 1 = %d] = %s.\n", to_index - 1, (parsed_links->array)[to_index - 1]);
                    //     sleep(4);
                    // }

                    possible_array[elements] = to_index;
                    elements++;
                    added++;
                }
                else
                {
                    // printf("New link not existing in preliminary list\n");
                }
                

            }

            // printf("\nExisting edges: \n");
            // for(j = 0; j < elements; j++)
            //     printf("%d ",possible_array[j]);
            // printf("\n");

            // if(index_under_wget >=2)
            //     sleep(1);

            // link = NULL;
        }


        free(possible_array);
        free(test_link);
        free(link);

        if(newlink)
            free(newlink);
    }
    // if(link)s

    if(added)
        printf("Number of links added = %d\n",added);

    fclose(ifp);
    fclose(filtered);

    // printf("end:\n");
    // print_links(parsed_links);
    // printf("\n");
    // // printf("to_index = %d\n",to_index );
    // printf("\n");
    // sleep(3);
}

void print_links(struct stringArray * list)
{
    int i;

    for (i = 0; i < list->size; i++)
    {
        printf("link #%d = \t%s\n",i+1,list->array[i] );
    }
}

int quick_search(struct stringArray * parsed_links, char * link)
{
    int i,j;
    int length = strlen(link);
    char * testlink;

    for(i = 0;i<parsed_links->size;i++)
    {
        testlink = (parsed_links->array)[i];
        // if(strstr(link,"aoc.mcgill.ca/site") != NULL)
        // {
            // printf("\n\n");
            // printf("testlink #%d = %s, link = %s\n",i+1,testlink,link );
            // sleep(1);

        // }
        if((int)strlen(testlink) == length)
        {
            for(j = 0; j < length; j++)
            {
                if(testlink[j] != link[j])
                {
                    break;
                }
            }

            if(j == length)
            {
                return i + 1;
            }
        }
    }

    return 0;
}

char * remove_extra(char * url)
{
    // printf("url = %s\n", url );

    char * result;


    // char * newurl = strstr(url,"//");

    // if(newurl == NULL)
    // {
    //     newurl = url;
    // }
    // else
    // {
    //     newurl = strstr(url,"//") + 2;
    // }

    int length = strlen(url);

    // we will make two tests: newline and slash, removing whatever appropriate
    int to_remove = 0;
    if (strchr(url,'\n') != NULL)
    {
        to_remove++;
        if (url[length-2] =='/')
        {
            to_remove++;
        }
    }
    else
    {
        if (url[length-1] =='/')
        {
            to_remove++;
        }
    }

    result = (char *)malloc(length- to_remove + 1);

    strncpy(result,url,length - to_remove);
    result[length - to_remove] = '\0';    

    return result;

}

void finish_up(struct stringArray * parsed, FILE * ofp, char * raw_links, char * filtered_links)
{

    pthread_t wget_finishing;
    int count, retry;
    char * link, *newurl;

    for(;index_under_wget <= parsed->size; index_under_wget++)
    {
        link = (parsed->array)[index_under_wget-1];

        printf("\n-----------------------\n\nRound %d of %d:\n", index_under_wget,(int)(parsed->size));
        printf("looking at link %s\n",link);
        newurl = switch_order(link);
        printf("reversal of this link is %s\n",newurl);
        free(newurl);

        count = 0;
        retry = 0;

        while(retry < RETRIES)
        {

            count = 0;

            thread_test = NOT_COMPLETED;
            pthread_create(&wget_finishing,NULL,wget_wrapper,link);

            while( (thread_test == NOT_COMPLETED) && (count < (WAITTIME*(retry+1))) )
            {
                usleep(100000);
                // printf("%d ",count );
                count++;
            }

            // printf("\n");

            if (count == WAITTIME)
            {
                pthread_cancel(wget_finishing);
                if(retry < RETRIES)
                    printf("Retrying...%d\n",retry);
                else
                    printf("Failed to retreive links\n");
                retry++;
            }
            else
            {
                pthread_join(wget_finishing, NULL);
                break;
            }
        }

        if (retry == RETRIES)
        {
            FILE * to_clear = fopen(raw_links,"w");
            fclose(to_clear);
        }

        // pthread_create(&wget_finishing, NULL,wget_wrapper,(parsed->array)[index_under_wget-1]);
        // pthread_join(wget_finishing,NULL);
        // wget_wrapper((parsed->array)[index_under_wget-1], raw_links);
        // sleep(10);
        read_and_save(raw_links, ofp, parsed,filtered_links, 0);
    }

}

void * wget_wrapper(void * arg)
{
    char * website = (char *)arg;
    // this is the template for the wget, we replace the x with the url we want to
    // crawl and the y at the very end with the location of the file we are dumping the
    // raw urls into
    char wget1[] = "wget -q \'";
    char wget2[] = "\' -O - | tr \"\\t\\r\\n'\" '   \"' | grep -i -o '\
<a[^>]\\+href[ ]*=[ \\t]*\"\\(ht\\|f\\)tps\\?:[^\"]\\+\"' |\
sed -e 's/^.*\"\\([^\"]\\+\\)\".*$/\\1/g' > "; 

    // find the relative locations of x an y in the template to determine where to splice
    // in the url and dump file
    // char * after_address = strchr(web_address,'x');
    // int first_copy = after_address - web_address;
    // after_address += sizeof(char);
    // char * after_file = strchr(web_address,'y');
    // int second_copy = after_file - after_address;
    // after_file += sizeof(char); 

    

    char * final_wget = (char *)malloc(strlen(wget1)+strlen(wget2)+strlen(website)+strlen(raw_links)+1);
    final_wget[0] = '\0';

    // build the full wget command

    strcat(final_wget,wget1);
    strcat(final_wget,website);
    strcat(final_wget,wget2);
    strcat(final_wget,raw_links);

    // int length = 0;
    // strncpy(final_wget + length*sizeof(char), web_address, first_copy);
    // length += first_copy;
    // strncpy(final_wget + length*sizeof(char), website,strlen(website));
    // length += strlen(website);
    // strncpy(final_wget + length*sizeof(char), after_address, second_copy);
    // length += second_copy;
    // strncpy(final_wget + length*sizeof(char), outputfile, strlen(outputfile));
    // length += strlen(outputfile);
    // strcpy(final_wget + length*sizeof(char), after_file);

    // call the wget
    // printf("%s\n",final_wget );

    printf("Running wget\n");
    system(final_wget);

    // sleep(1);

    free(final_wget);

    thread_test = COMPLETED;

    return NULL;

}

char * switch_order(char * url)
{

    char ** pieces = (char **)malloc(sizeof(char *)*3);
    char * stops = (char *)malloc(sizeof(char)*3);

    // http://(www.google.com) or http://(google.com)
    char * beginning = strstr(url, "//");
    char * middle, * end, *path, * newurl, * testurl;

    testurl = NULL;

    if(beginning == NULL)
    {
        // if there isn't http(s):// for some reason
        free(pieces);
        free(stops);
        if(testurl != NULL)
            free(testurl);
        return NULL;
    }
    else
    {
        beginning = &(beginning[2]);
    }

    // printf("%s %s %s\n",beginning,(strchr(strchr(beginning,'.')+1,'.')),strchr(beginning,'/'));

    if( (strchr(strchr(beginning,'.')+1,'.') == NULL) || 
        ((strchr(strchr(beginning,'.')+1,'.') > strchr(beginning,'/')) &&
        (strchr(strchr(beginning,'.')+1,'.') != NULL) && (strchr(beginning,'/') != NULL)))
    {
        // printf("here1\n");
        // we have a link with only one period
        // add a www to the beginning of beginning
        testurl = (char *)malloc(strlen("www.") + strlen(beginning) + 1);     
        testurl[4 + strlen(beginning)] = '\0';

        strcpy(testurl,"www.");
        strcpy(&(testurl[4]),beginning);
        testurl[4 /* "www." */ + strlen(beginning)] = '\0';
        beginning = testurl;
    }

    int final_length = (int)(strlen(beginning)) +1;

    // find the next location for the slash, as this seperates the part
    // of the link we want to switch from the rest we want to leave alone

    // www.google.com(/asndalkndasa) or google.com(NULL)
    char * seperator = strchr(beginning,'/');

    // this will be our first split in the code: either there is a / after the main link or 
    // we just have the basic link
    if(seperator == NULL)
    {
        // if the slash does not exist (e.g http://google.com or http://www.google.com )
        seperator = "\0";

        // www.(google.com) or google.(com)
        middle = strchr(beginning,'.') + 1;

        if (middle == NULL)
        {
            // there were no periods in the link, WTF?
            free(pieces);
            free(stops);
            if(testurl != NULL)
                free(testurl);
            return NULL;
        }

        // www.google.(com) or google.com(NULL)
        end  = strchr(middle,'.') + 1;

        if(end == NULL)
        {
            // there was no second period, so we have to re-evaluate what we think of
            // as beginning, middle and end
            end = middle;   //(com)
            middle = beginning; //(google.com)
            beginning = "www.";            
            final_length += 4;

        }
    }
    else
    {
        //we do have a character that separates the main link from the path

        // www.(google.com/sadasdsa) or www.(google.com/) or google.(com/asda.ssa) or google.(com/)
        middle = strchr(beginning,'.') + 1;

        if (middle == NULL)
        {
            // there were no periods in the link, WTF?
            free(pieces);
            free(stops);
            if(testurl != NULL)
                free(testurl);
            return NULL;
        }

        // www.google.(com/sadasdsa) or www.google.(com/) or google.com/(NULL) or google.com/adasds.(asdads)
        end  = strchr(middle,'.') + 1;

        if((end == NULL) || ((int)(end-seperator) > 0))
        {
            // there was no second period or it's a vestigal period in the path
            // so we have to re-evaluate what we think of as beginning, middle and end
            end = middle;   //(com/asdadasd) or (com/asdsa.asdasd) or (com/)
            middle = beginning; //(google.com/asdsadsa.asdsda)
            beginning = "www."; 
            final_length += 4;

        }


        seperator = "/";

    }

    // for strange links like www.alumni.mcgill.ca with a useless (www.)
    if( (strchr(end,'/') - strchr(end,'.') > 0) && (strchr(end,'.') != NULL))
    {
        // bump everything up one
        beginning = middle;         //(www.alumni.mcgill.ca) --> www.(alumni.mcgill.ca)
        middle = end;               //www.(alumni.mcgill.ca) --> www.alumni.(mcgill.ca)
        end = strchr(end,'.') + 1;  //www.alumni.(mcgill.ca) --> www.alumni.mcgill.(ca)
    }


    newurl = (char *)malloc(final_length);

    // set up both the pieces of the url we will look at and the characters that we will look for
    // in order to shift to the next pointer
    pieces[0] = end;            //(com)
    pieces[1] = middle;         //(google.com)
    pieces[2] = beginning;      //(www.)

    // printf("\n%s\n",end );
    // printf("%s\n", middle);
    // printf("%s\n\n",beginning );
    //     sleep(1);
    // }



    stops[0] = seperator[0];
    stops[1] = '.';
    stops[2] = '.';

    int pieces_stops_count = 0;
    int url_count = 0;
    int i;

    do
    {
        //step through each of the pieces
        i = 0;

        // add each of the characters from this pieces until we find a character that matches
        // the stopping character for this piece
        while(pieces[pieces_stops_count][i] != stops[pieces_stops_count])
        {
            newurl[url_count++] = pieces[pieces_stops_count][i++];
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        }

        // printf("\n");

        if(pieces_stops_count < 2)
        {
            newurl[url_count++] = '.';
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        }

        // move to the next piece
        pieces_stops_count++;

    }while(pieces_stops_count < 3);


    // printf("\n");

    if(stops[0] == '/')
    {
        // http://(www.google.com(/jasdkadksa)) or http://(google.com/(asdsadas))
        path = strstr(url, "//") + 2;
        path = strchr(path,'/');


        i = 0;

        // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        do
        {
            // copy the remainder of the path to the new url
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
            newurl[url_count++] = path[i++];
        }while(path[i] != '\0');
    } 

    // copy the terminator
    newurl[url_count] = '\0';

    //clean up;
    free(pieces);
    free(stops);
    if(testurl != NULL)
        free(testurl);

    // for(i = 0; i < strlen(newurl); i++)
    // {
    //     if( (newurl[i] > '~') || (newurl[i] < '!') )
    //     {
    //         // printf("Error with \"%s\"\nOffending character: newurl[%d] = %c, length = %d\n",newurl, i,newurl[i], (int)(strlen(newurl)) );
    //         return NULL;
    //     }
    // }

    return newurl;
}