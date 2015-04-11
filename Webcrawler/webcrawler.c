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
#define APPEND          1
#define NOAPPEND        0

struct link_and_index
{
    int * index_ptr;
    char * str;
};

struct stringArray
{
    struct link_and_index * array;
    size_t size;
};

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
void read_and_save(char * raw_links, struct node * node_under_wget, struct stringArray * links_to_crawl,
    int should_append);
int ** quick_search(struct stringArray * parsed_links, char * link);
char * remove_slash(char ** link);
// void print_links(struct stringArray * list);
char * redirected(char * original);
int first_after_second(char * a, char * b);
void add_link_string(struct node * n, char * link);
void remove_links_and_edges(struct Linked_list *a);
void free_LL(struct Linked_list * a);
struct node * add_sorted_link(struct Linked_list * parsed_links, char * str);
void crawl(struct node * node_under_wget, struct stringArray * links_to_crawl, char * link, int should_append);

int index_under_wget = 1;
char raw_links[] = "raw_links.txt";

int thread_test;

int main(int argc, char *argv[])
{
    struct link_and_index * link = (struct link_and_index *)malloc(sizeof(struct link_and_index)); 
    struct stringArray * links_to_crawl = (struct stringArray *)malloc(sizeof(struct stringArray));
    struct Linked_list * parsed_links = initialize_linked_list(0);
    char * filtered = "filtered_links.txt"; // where we will store the final, sorted list of links
    char * output = "output.txt";       // where we will store the edges which describe the graph
    struct node * node_under_wget;
    struct node * temp;
    int i;

    FILE * ofp = fopen(output,"w");
    if(ofp == NULL)
    {
        printf("Could not open %s for writing\n", output);
        return(-1);
    }

    FILE * filtered_file = fopen(filtered,"w");
    if(filtered_file == NULL)
    {
        printf("Could not open %s for writing\n", filtered);
        return(-1);
    }
    printf("here\n");

    //find the link we will start the crawl with
    if(argc == 2)
    {
        link->str = argv[1];
    }
    else
    {
        link->str = "http://www.google.com";
    }

    links_to_crawl->size = 1;
    links_to_crawl->array = (struct link_and_index *)malloc((int)(links_to_crawl->size) * sizeof(struct link_and_index));
    links_to_crawl->array[0].str = link->str;


    // crawl as many links as the value of STARTLINKS dictates and append all these links'
    // sublinks to the list of links that we will eventually crawl
    do
    {           
        // find the next link with it's string and index
        link = &(links_to_crawl->array[index_under_wget-1]);
        printf("\n-----------------------\n\nRound %d of %d:\n", index_under_wget,(int)(links_to_crawl->size));
        printf("looking at link %s\n",link->str); 
        // find where it belongs, lexigraphicaly, in the linked list and return the pointer to that node
        node_under_wget = add_sorted_link(parsed_links,link->str);
        if(node_under_wget == NULL)
        {
            // somehow, after all our filtering, this link already exists in the parsed linked list
            exit(0);
        }
        // update the pointer in the links_to_crawl which points to the index of this node
        link->index_ptr = &(node_under_wget->index);
        // crawl this link for all the hyperlinks on its page
        crawl(node_under_wget, links_to_crawl,link->str,APPEND);
        // printf("root after crawl = %s\n", parsed_links->root->filtered_hyperlink );
        // move to the next link
        index_under_wget++;
    }while(index_under_wget <= STARTLINKS);

    // sleep(1);
    // printf("root between crawls = %s\n", parsed_links->root->filtered_hyperlink );


    // after this initial number of links to crawl has been acheived, continue crawling
    // the links in the links_to_crawl list, but no longer append the links they point to
    // (this is the finishing up phase);
    while(index_under_wget <= links_to_crawl->size)
    {
        // find the next link with it's string and index
        link = &(links_to_crawl->array[index_under_wget-1]);
        printf("\n-----------------------\n\nRound %d of %d:\n", index_under_wget,(int)(links_to_crawl->size));
        printf("looking at link %s\n",link->str); 
        // find where it belongs, lexigraphicaly, in the linked list and return the pointer to that node
        node_under_wget = add_sorted_link(parsed_links,link->str);
        if(node_under_wget == NULL)
        {
            // somehow, after all our filtering, this link already exists in the parsed linked list
            exit(0);
        }
        // update the pointer in the links_to_crawl which points to the index of this node
        link->index_ptr = &(node_under_wget->index);
        // crawl this link for all the hyperlinks on its page
        crawl(node_under_wget, links_to_crawl,link->str,NOAPPEND);
        // move to the next link
        index_under_wget++;
    }

    // print all the ordered links to file and, at the same time, print their edges to a seperate
    // file
    temp = parsed_links->root;
    do
    {
        fprintf(filtered_file, "%s\n",temp->hyperlink);
        for(i = 0; i < temp->edges.size; i++)
        {
            fprintf(ofp, "%d\t%d\n", temp->index,**(temp->edges.array[i]));
        }
        to_next(&temp);
    }while(temp != parsed_links->root);

    free(link);
    free(links_to_crawl->array);
    free(links_to_crawl);
    free_LL(parsed_links);

    fclose(filtered_file);
    fclose(ofp);

    /* we're done with libcurl, so clean it up */ 
    curl_global_cleanup();

    return 0;

}








//
//functions


void crawl(struct node * node_under_wget, struct stringArray * links_to_crawl, char * link, int should_append)
{
    pthread_t wget;
    int count, retry = 0;

    while(retry < RETRIES)
    {
        // we want to try several times to use wget, with increasingly long wait times

        count = 0;

        // set a value that we will wait to see if the thread has altered
        thread_test = NOT_COMPLETED;
        //start the thread which will input all the links on link's page into the raw links file
        pthread_create(&wget,NULL,wget_wrapper,link);

        // wait for either a timeout or a change in the external value by the thread
        while((thread_test == NOT_COMPLETED) && (count < (WAITTIME*(retry+1))))
        {
            usleep(100000);
            count++;
        }


        if (count == WAITTIME)
        {
            // we've timed out
            // cancel and then either try again if we have more retries, or stop attempting
            // to access this site
            pthread_cancel(wget);
            if(retry < RETRIES)
                printf("Retrying...%d\n", retry);
            else
                printf("Failed to retreive links\n");
            retry++;
        }
        else
        {
            // the thread completed successfully
            // free the memory
            pthread_join(wget,NULL);
            break;
        }
    }

    if (retry == RETRIES)
    {
        // we don't want a file with half finished or error ridden links
        // so we clear it of it's contents so that read_and_save will know not to bother with
        // this link's out edges
        FILE * to_clear = fopen(raw_links,"w");
        fclose(to_clear);
    }

    // using the links we just found, the linked list of crawled links and the list of links we will
    // soon crawl, parse the liks
    read_and_save(raw_links, node_under_wget,links_to_crawl, should_append);

}

void read_and_save(char * raw_links, struct node * node_under_wget, struct stringArray * links_to_crawl,
    int should_append)
{
    // open the raw links file from wget
    FILE * ifp = fopen(raw_links,"r");
    char link[2500];
    char * newlink = NULL;
    int ** to_index_ptr;
    int i;

    if(ifp == NULL)
    {
        printf("Could not open %s for writing\n", raw_links);
        exit(0);
    }

    // determine how many links were found through themost recent wget
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


    // we can either refrain from adding links to crawl later or include them, based on the
    // value from the calling function. Edges will be added regardless
    if(should_append == APPEND)
    {
        printf("Adding edges and appending links\n");
    }
    else
    {   
        printf("Adding edges\n");
    }

    if(lines)
    {
        // only bother with the rest of this code if at least one link was found

        // go back up the the top of the page to run through the lines again
        rewind(ifp);

        // we want to keep track of which edges have been added
        // this will also be the array of double pointers for the link being crawled
        int *** possible_array = (int ***)malloc(sizeof(int**)*lines);

        char * redirected_link = NULL;
        char * test_link = NULL;

    
        while(fgets(link,2500,ifp) != NULL)
        {
            // while there are lines left in the file to parse, keep parsing


            free(newlink);
            free(test_link);
            newlink = NULL;
            test_link = remove_extra(link);

            // determine if the links is redirected (for shortened urls)
            if((strstr(test_link,"//goo.gl") != NULL) || (strstr(test_link,"//tinyurl") != NULL) ||
                (strstr(test_link,"//t.co") != NULL) || (strstr(test_link,"//x.co") != NULL)||
                (strstr(test_link,"//is.gd") != NULL) || (strstr(test_link,"//bit.ly") != NULL))
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
            
            // find if the link we are looking at already exists in the links we have crawled/will crawl
            // and obtain a pointer to a pointer to the index
            to_index_ptr = quick_search(links_to_crawl,newlink);

            if(to_index_ptr != NULL)
            {
                //we have found a link that has already been added to the list in a previous crawl

                for (i = 0; i < elements; i++)
                {
                    // determine if we already added this link to the edges for the link being crawled
                    if(to_index_ptr == possible_array[i])
                        break;
                }

                if (i == elements)
                {
                    // we have not added this link yet to the list of edges for this node
                    possible_array[elements] = to_index_ptr;
                    elements++;
                }
            }
            else
            {
                // this is a new link that does not already exist in our list

                if (should_append == APPEND)
                {
                    // only add the link to the newly discovered node if we are in the initial stages of
                    // link apprehension

                    // make room for the new link
                    links_to_crawl->size += 1;
                    links_to_crawl->array = (struct link_and_index *)realloc(links_to_crawl->array, 
                        (int)(links_to_crawl->size)*sizeof(struct link_and_index));

                    // if((links_to_crawl->array)[(int)(links_to_crawl->size)-1] == NULL)
                    // {
                    //     printf("Error in allocating memory for link\n");
                    // }

                    // add the link and it's (NULL to begin with) pointer to the index to the end
                    // of the links_to_crawl
                    (links_to_crawl->array)[(int)(links_to_crawl->size)-1].index_ptr = NULL;
                    (links_to_crawl->array)[(int)(links_to_crawl->size)-1].str = (char*)malloc((int)(strlen(newlink))+1);
                    strcpy((links_to_crawl->array)[(int)(links_to_crawl->size)-1].str, newlink);

                    possible_array[elements] = &((links_to_crawl->array)[(int)(links_to_crawl->size)-1].index_ptr);
                    elements++;
                    added++;
                }
            }
        }


        node_under_wget->edges.array = possible_array;
        node_under_wget->edges.size = elements;

        if(newlink)
            free(newlink);
    }

    if(added)
        printf("Number of links added = %d\n",added);

    fclose(ifp);
}

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

        /* always cleanup */ 
        curl_easy_cleanup(curl);


    }
    else
    {
        printf("cURL error.\n");
    }

    return result;

}

// void print_links(struct stringArray * list)
// {
//     int i;

//     for (i = 0; i < list->size; i++)
//     {
//         printf("link #%d = \t%s\n",i+1,(list->array[i]).str );
//     }
// }

int ** quick_search(struct stringArray * links_to_crawl, char * link)
{
    // search for the link we have been given in the list of links we have already parsed
    //or will soon parse. If we find that link in the list, it will have associated with it
    // a pointer to the index in the node in linked list of links we have already crawled. 
    // return a pointer to that pointer. We return a pointer to a pointer because, while some links
    // in the links_to crawl list have already been crawled, most likely have not and their location
    // will fluctuate wildly as sorting occurs. So if we return a pointer to that pointer, in this case it
    // will initially be pointing to a null variable, but as soon as this link is crawled, it will have a
    // pointer to the actual index of the link and thus we can dereference this pointer twice to get to
    // the actual index

    int i,j;
    int length = strlen(link);
    struct link_and_index * testlink;

    for(i = 0;i<links_to_crawl->size;i++)
    {
        testlink = &((links_to_crawl->array)[i]);
        // if(strstr(link,"aoc.mcgill.ca/site") != NULL)
        // {
            // printf("\n\n");
            // printf("testlink #%d = %s, link = %s\n",i+1,testlink,link );
            // sleep(1);

        // }
        if((int)strlen(testlink->str) == length)
        {
            for(j = 0; j < length; j++)
            {
                if(testlink->str[j] != link[j])
                {
                    break;
                }
            }

            if(j == length)
            {
                return &(testlink->index_ptr);
            }
        }
    }

    return NULL;
}

char * remove_extra(char * url)
{
    // remove any extra characters at the end of the string like a new lines or a slash
    // these may cause problems later on so we get rid of them right away

    char * result;

    int length = strlen(url);

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

        strcat(testurl,"www.");
        strcat(&(testurl[4]),beginning);
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

int first_after_second(char * a, char * b)
{
    // returns 1 if "a" lexicographically comes after "b",
    //  0 if "b" comes after "a" and 3 if they are the same
    // string
    if(a[0] == '\0' && b[0] != '\0')
    {
        return 0;
    }
    else if(b[0] == '\0' && a[0] != '\0')
    {
        return 1;
    }
    else if(b[0] == '\0' && a[0] == '\0')
    {
        return 3;
    }
    else
    {
        if(a[0] > b[0])
        {
            return 1;
        }
        else if(a[0] < b[0])
        {
            return 0;
        }
        else
        {
            return first_after_second(&(a[1]),&(b[1]));
        }
    }
}

void add_link_string(struct node * n, char * link)
{
    n->hyperlink = (char*)malloc(strlen(link)+1);
    strcpy(n->hyperlink, link);
    n->filtered_hyperlink = switch_order(link);
}

void remove_links_and_edges(struct Linked_list *a)
{
    struct node * temp = a->root;

    do
    {
        free(temp->hyperlink);
        free(temp->filtered_hyperlink);
        if((int)(temp->edges.size) > 0)
            free(temp->edges.array);
        to_next(&temp);
    } while (temp != a->root);
}

void free_LL(struct Linked_list * a)
{
    remove_links_and_edges(a);
    delete_linked_list(a);
    free(a);
}

struct node * add_sorted_link(struct Linked_list * parsed_links, char * str)
{
    // struct node * temp = parsed_links->root;   
    // int test;   // to test the order

    if(parsed_links->root == NULL)
    {
        //this is the first link that we are inputing into
        // the linked list so we don't need to care about anything really
        // just put it at the front and don't worry about sorting
        add_node(parsed_links);
        parsed_links->root->index = 1;
        add_link_string(parsed_links->root,str);
        return parsed_links->root;
    }
    else
    {
        struct node * new_node = (struct node*)malloc(sizeof(struct node));   // this will hold all the link's node info
        struct node * temp = parsed_links->root; // to loop through the list
        add_link_string(new_node,str);
        int test;
        // there are other nodes in the list, so we seek out this link's place in the list
        do
        {   
            test = first_after_second(temp->filtered_hyperlink,new_node->filtered_hyperlink);
            if(test == 1)
            {   
                //we've determined that the str, lexigraphically, comes before temp.
                // thus we insert it before temp
                new_node->index = temp->index;
                if(new_node->index == 1)
                {
                    // must move the root if this is the first link
                    parsed_links->root = new_node;
                }

                // push the node to the proper location (just before the last node we tested)
                insert_node_before(temp,new_node);
                parsed_links->size += 1;

                if (temp == parsed_links->root)
                {
                    parsed_links->root = new_node;
                }

                do
                {
                    // incriment all the indices of the links after this node
                    temp->index += 1;
                    to_next(&temp);
                } while (temp != parsed_links->root);

                //we're done
                return new_node;
            }
            else if (test == 3)
            {
                // the links appear to be the same, but it might just be the filtered hyperlinks,
                // implying that one is https and the other http
                if(strstr(temp->hyperlink,"https")!= NULL && strstr(new_node->hyperlink,"https") == NULL)
                {
                    //we've determined that the str, lexigraphically, comes before temp.
                    // thus we insert it before temp
                    new_node->index = temp->index;
                    if(new_node->index == 1)
                    {
                        // must move the root if this is the first link
                        parsed_links->root = new_node;
                    }

                    // push the node to the proper location (just before the last node we tested)
                    insert_node_before(temp,new_node);
                    parsed_links->size += 1;

                    if (temp == parsed_links->root)
                    {
                        parsed_links->root = new_node;
                    }

                    do
                    {
                        // incriment all the indices of the links after this node
                        temp->index += 1;
                        to_next(&temp);
                    } while (temp != parsed_links->root);

                    //we're done
                    return new_node;
                } 
                else if (strstr(temp->hyperlink,"https") == NULL && strstr(new_node->hyperlink,"https") != NULL)
                {
                    // it's the opposite, with the new link being the secure link and thus coming after the
                    // link already in the list
                    new_node->index = temp->index + 1;

                    // push the node to the proper location (just after the last node we tested)
                    insert_node_after(temp,new_node);
                    parsed_links->size += 1;

                    to_next(&temp);
                    while (temp != parsed_links->root)
                    {
                        // incriment all the indices of the links after this node
                        temp->index += 1;
                        to_next(&temp);
                    }

                    //we're done
                    return new_node;
                } 
                else
                {
                    // we should not have made it this far if the link already exists in the list
                    // therefor there has been an error in a previous function
                    printf("----------------------------\n");
                    printf("Error: repeated link %s from %s\n",new_node->filtered_hyperlink, new_node->hyperlink);
                    printf("is the same as %d: %s from %s\n",(int)(temp->index),temp->filtered_hyperlink, 
                        temp->hyperlink);
                    printf("input link is \"%s\"\n",str );
                    printf("---------------------------\n");
                    free(new_node->hyperlink);
                    free(new_node->filtered_hyperlink);
                    free(new_node);
                    return NULL;
                    // exit(0);
                }
            }
            else
            {
                // the link in temp comes before the link under test, str
                // move to the next link in the list
                to_next(&temp);
            }
        }while (temp != parsed_links->root);

        if(temp == parsed_links->root)
        {
            // we've made it to the point where we know that this new link is higher, lexigraphically, than all the other links
            // so we add it to the end
            new_node->index = parsed_links->root->prev->index + 1;
            append_node(parsed_links,new_node);
            return new_node;
        }
    }

    return NULL;
}
