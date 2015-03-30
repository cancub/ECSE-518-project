// credit to user Jay Taylor for the wget call used here to retreive hyperlinks from a webpage
// http://stackoverflow.com/a/2804721

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "linked_list.h"

#define BUFFLEN 2500

// NOTE THAT THE FORMAT FOR FILTERED LINKS IS 
// (completely filtered, extra removed, index of page that pointed to this one)

// WHILE THE FULLY PARSED LINKS ARE STORED AS
// (completely filtered, extra removed, this page's index)

//http://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

char * flip_url(char * url);
char * add_slash(char * url);
void wget_wrapper(char * website, char * outputfile);
void crawl_page(char * inputfile, char * newinputfile, char * parsed_links, char * raw_links);
char * remove_extra(char * url);
char * append_directory(char * filename);
void filter_and_store(char * raw, struct Linked_list * a);
char * get_next_link(struct Linked_list * parsed);
void add_edge(struct Linked_list * a, int from_index, int to_index);
void add_link(struct Linked_list * a, char * link, int index);
char * clean_link(char * url);
int search_for_link(struct Linked_list * a, char * filtered_link);
void print_string(char * string_thing);
char * rm_invalid(char * url);
void finish_up(struct Linked_list * parsed);

int index_under_wget = 1;
char raw_links[] = "raw_links.txt";

int main()
{
    char * link; 
    struct Linked_list * parsed_links;
    struct node * temp;
    FILE * ofp = fopen("output.txt","w");

    char starter[] = "http://www.mcgill.ca";    // the starting node

    //initialize linked lists
    parsed_links = initialize_linked_list(1);

    link = starter;

    add_link(parsed_links,link,index_under_wget);

    do
    {        

        printf("looking at link %s at index %d\n",link, index_under_wget);

        wget_wrapper(link, raw_links);

        filter_and_store(raw_links, parsed_links);

        index_under_wget++;

        link = (get_node_at_index(parsed_links,index_under_wget-1))->hyperlink;

        // print_linked_list(parsed_links);

    }while(index_under_wget <= 80);



    finish_up(parsed_links);


    // printf("Current parsed list:\n");
    // print_linked_list(parsed_links);

    temp = parsed_links->root;
    int i;

    struct TwoDArray graph;
    construct_2DArray(&graph);

    do
    {   
        add_array(&graph);
        fprintf(ofp, "%s %d ", temp->filtered_hyperlink, temp->data);
        if(temp->edges.array[0] != -1)
        {
            fprintf(ofp, "[");
            for(i = 0; i < temp->edges.size; i++)
            {
                add_element(&(graph.array[(temp->data - 1)]),temp->edges.array[i]);

                if(i < temp->edges.size - 1)
                {
                    fprintf(ofp, "%d,",temp->edges.array[i] );
                }
                else
                {
                    fprintf(ofp, "%d] ",temp->edges.array[i] );
                }
            }
        }
        temp = temp->next;
        fprintf(ofp, "\n");
    }while(temp != parsed_links->root);

    // print_2DArray(&graph);
    print_2DArray_to_file(&graph, "mcgill_graph.txt");

    delete_linked_list(parsed_links);
    delete_2DArray(&graph);

    return 0;

}








//
//functions




void finish_up(struct Linked_list * parsed)
{
    FILE * ifp;    
    char link_to_filter[BUFFLEN];
    int index;
    char * link_to_wget;

    //  while there are still links in the queue, we want to crawl each
    //  and find which of the links in the current parsed list that it points to
    //  and them append this node to the end of the parsed list
    while(index_under_wget < (parsed->size + 1))
    {   
        // get the next link
        link_to_wget = (get_node_at_index(parsed,index_under_wget-1))->hyperlink;
        wget_wrapper(link_to_wget, raw_links);

        printf("link = %s, number %d of %d\n",link_to_wget, index_under_wget, parsed->size);
        sleep(1);

        ifp = fopen(raw_links,"r");

        if(ifp == NULL)
        {
            printf("Could not open %s for writing\n", raw_links);
            exit(0);
        }

        while(fscanf(ifp,"%s", link_to_filter) == 1)
        {

            if((strstr(link_to_filter,".") != NULL) && (strstr(link_to_filter,"//") != NULL))
            {

                index = search_for_link(parsed,clean_link(link_to_filter));
                if(index < parsed->size + 1)
                {
                    add_edge(parsed, index_under_wget, index);
                }
                // printf("Current queue:\n");
                // print_linked_list(a);
                // printf("\n");
                // sleep(1);
            }
        }; 

        fclose(ifp);

        index_under_wget++;
    }
}

int search_for_link(struct Linked_list * a, char * filtered_link)
{
    // search through the linked list for a node which contains the same filtered_hyperlink
    // member as the test filtered_link and then return the index of this link.
    // if the link cannot be found, return the size of the array + 1 to let the calling
    // function know that the link did not exist in the array. this also will act 
    // as the index of the new link.

    struct node * test;
    test = a->root;
    // int charnum;

    

    if (a->size > 0)
    {
        // we don't need to search for the link if the list is empty
        do  
        {   
            

            if ( strcmp(test->filtered_hyperlink,filtered_link) == 0)
            {
                // printf("%s exixts already, test->data = %d\t\t**********\n", filtered_link, test->data);
                // sleep(1);
                return test->data;
            }
            to_next(&test);
        }while(test != a->root);
    }

    // printf("%s does not exist\n", filtered_link);    

    return (a->size + 1);
}

char * clean_link(char * url)
{
    // char * test = flip_url(url);

    // test = remove_extra(test);

    // test = add_slash(test);

    return rm_invalid(add_slash(remove_extra(flip_url(url))));
}

char * rm_invalid(char * url)
{
    //remove invalid characters from the url that may have been added at some point
    // it's a weak fix, but screw it.
    int i = 0;

    for(i = 0; i < strlen(url); i++)
    {
        if((int)(url[i]) < 33 || (int)(url[i]) > 126)
        {

            break;
        }
    }

    if(i < strlen(url))
    {
        strcpy(url+i,url+i+1);
    }

    // if(strstr(url,"wordpress") != NULL)
    // {
    //     print_string(url);
    //     sleep(1);
    // }

    return url;
}

void print_string(char * string_thing)
{
    int i;

    for(i = 0; i < strlen(string_thing); i++)
    {
        printf("%c\n", string_thing[i]);
    }
}

void filter_and_store(char * raw, struct Linked_list * a)
{        

    
    FILE * ifp = fopen(raw_links,"r");
    char link_to_filter[2048];

    if(ifp == NULL)
    {
        printf("Could not open %s for writing\n", raw_links);
        exit(0);
    }

    // get the next link from the file of raw links
    while(fscanf(ifp,"%s", link_to_filter) == 1)
    {   
        // attempt to add the link to the parsed list


        add_link(a,link_to_filter, index_under_wget);
    }; 

    fclose(ifp);
}

void add_link(struct Linked_list * a, char * link, int from_index)
{


    // printf("here with link %s\n", link);

    // printf("testing %s\n",link );

    if((strstr(link,".") != NULL) && (strstr(link,"//") != NULL))
    {
        char * cleanlink = clean_link(link);
        int new_index = search_for_link(a,cleanlink);

        // if(strcmp(cleanlink,"ca.mcgill.royalvictoria/") == 0)
        // {
        //     printf("new_index = %d, a->size = %d\n", new_index, a->size);
        // }

        if(new_index < a->size + 1)
        {
            // this link already exists in the parsed list
            // therefore we do not need to add it, but will instead add the
            // edge from the page that we just performed wget on (from_index) 
            // to the index of the page we just found (new_index)

            add_edge(a,from_index,new_index);        
        }
        else if(new_index == (a->size + 1))
        {
            // the link does not currently exist in our parsed list. so
            // we give the link a number, create a node for it, add the edge
            // from <from_index> to this new number, and add this node to the end
            // of the parsed list
            struct node * temp = (struct node *)malloc(sizeof(struct node));
            char * new_link = remove_extra(link);
            char * filtered_link = cleanlink;
            temp->data = new_index;

            temp->hyperlink =(char *)malloc(strlen(new_link) + 1);
            strcpy(temp->hyperlink, new_link);
            temp->filtered_hyperlink = (char *)malloc(strlen(filtered_link) + 1);
            strcpy(temp->filtered_hyperlink, filtered_link);

            construct_Array(&(temp->edges));

            add_edge(a,from_index,new_index);
        
            append_node(a,temp);
        }
        else
        {
            printf("something went wrong\n");
        }
    }
}

void add_edge(struct Linked_list * a, int from_index, int to_index)
{

    // this function is called when an edge is being added to one of the
    // links that exists in the parsed linked list
    struct node * temp = a->root;

    do
    {
        if(temp->data == from_index)
        {

            if(search_Array(temp->edges, to_index) == 0)
            {
                add_element(&(temp->edges),to_index);                
            }
            
            break;
        }

        temp = temp->next;
    }while(temp != a->root);
}

// char * append_directory(char * filename)
// {
//     // printf("%s\n", filename);
//     char cwd[1024];
//     getcwd(cwd, sizeof(cwd));
//     // printf("length of cwd = %d, string length of cwd = %d\n",(int)(sizeof(cwd)), (int)(strlen(cwd)) );
//     char * result = (char *)malloc(sizeof(char)*(strlen(cwd) + strlen(filename)));
//     strcat(result,cwd);
//     strcat(result,"/text_files/");
//     strcat(result,filename);
//     // printf("result = %s\n", result);
//     return result;
// }


char * remove_extra(char * url)
{
    
    printf("url = %s\n", url );

    if (strchr(url,'?') != NULL)
    {
        // printf("before = %s\n",url);
        url[strchr(url,'?')-url] = '\0';
        // printf("after = %s\n",url);
    }

    return url;
}

void wget_wrapper(char * website, char * outputfile)
{

    // this is the template for the wget, we replace the x with the url we want to
    // crawl and the y at the very end with the location of the file we are dumping the
    // raw urls into
    char wget1[] = "wget -q ";
    char wget2[] = " -O - | tr \"\\t\\r\\n'\" '   \"' | grep -i -o '\
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

    char final_wget[strlen(wget1)+strlen(wget2)+strlen(website)+strlen(outputfile)+1];

    // build the full wget command

    strcat(final_wget,wget1);
    strcat(final_wget,website);
    strcat(final_wget,wget2);
    strcat(final_wget,outputfile);

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
    printf("%s\n",final_wget );
    system(final_wget);

}

char * add_slash(char * url)
{
    char * result = (char *)malloc(sizeof(char) * (strlen(url) + 2));

    int i = 0;

    while(url[i] != '\0')
    {
        i++;
    };

    if (url[i-1] != '/')
    {
        strcpy(result,url);
        strcpy(result + strlen(url), "/\0");
    }
    else
    {
        result = url;
    }

    return result;
}




char * flip_url(char * url)
{


    int length = strlen(url);

    char * host, * top_level_domain, * end_of_string, * middle, * filepath, * result;   
    int tld_size, middle_size, hostname_size;

    char * test = url;

    char * new_host = (char *)malloc(length*sizeof(char) + 1);
    


    //search for either the '\0' string terminator or the first / to start the path portion
    // of the url

    do 
    {
        test = strchr(test + sizeof(char),'/');
        // printf("test = %s\n", test);
        // sleep(1);
    }while ((test != NULL) && (((test[-1] == '/') || (test[1] == '/'))));

    result = test;

    test = url;

    if( strchr(&(strchr(url, '.')[1]),'.') == NULL )
    {
        //there was only one dot in the url, meaning that we have something like
        // http://google.com/

        host = (char *)malloc(3*sizeof(char));
        host[0] = host[1] = host[2] = 'w';
        hostname_size = 3;



        if(result == NULL)
        {
            end_of_string = strchr(url,'\0');

            top_level_domain = &((strchr(url,'.'))[1]);

            // printf("tld = %s\n", top_level_domain);

            tld_size = end_of_string - top_level_domain;

        }
        else
        {
            do
            {
                // printf("here\n");
                //find the last dot before the path. this immediately precedes the top level domain.
                test = strchr(&(test[1]),'.');
                // printf("test = %s\n", test);
            }
            while((strchr(&(test[1]),'.') != NULL) && (strchr(&(test[1]),'.') - result < 0));

            top_level_domain = &(test[1]);

            // printf("tld = %s\n", top_level_domain);

            // find the tld length
            tld_size = result - top_level_domain;

            test = url;
        }

        do 
        {
            test = strchr(test + sizeof(char),'/');
            // printf("test = %s\n", test);
            // sleep(1);
        }while (test[-1] != '/');

        test = &(test[1]); //starts with the hostname

        middle = (char *)malloc((length+1)*sizeof(char));
        middle[length] = '\0';
        middle[0] = '.';
        strcpy(middle + sizeof(char), test);
        middle_size = top_level_domain - test + 1;

        // printf("middle = %s\n", middle);

        // printf("hostname_size = %d, tld_size = %d, middle_size = %d\n", hostname_size,tld_size,middle_size);

        strncpy(new_host, top_level_domain, tld_size);
        // printf("new_host = %s\n", new_host );
        strncpy(new_host + sizeof(char)*tld_size,middle,middle_size);
        // printf("new_host = %s\n", new_host );
        strncpy(new_host + sizeof(char)*(tld_size+middle_size),host,hostname_size);
        // printf("new_host = %s\n", new_host );

        if(result !=NULL)
        {
            strcpy(new_host + sizeof(char)*(tld_size+middle_size+hostname_size), result);
            // printf("new_host = %s\n", new_host );
        }




        return new_host;
    }







    if ( result == NULL)
    {
        // printf("the test was null\n");
        // there was no filepath, just the main site
        end_of_string = strchr(url,'\0');
        test = url;
        do
        {
            //find the last dot. this immediately precedes the top level domain.
            test = strchr(&(test[1]),'.');
        }
        while(strchr(&(test[1]),'.') != NULL);

        top_level_domain = &(test[1]);
        // printf("tld = %s\n", top_level_domain);

        // find the tld length
        tld_size = end_of_string - top_level_domain;

        test = url;

        do
        {   
            //find the second last dot. this is what seperates the hostname
            //from the remainder of the host
            test = strchr(&(test[1]),'.');
        }while( strchr(&(strchr(&(test[1]),'.')[1]),'.') != NULL);

        middle = test;
        middle_size = top_level_domain - middle;
        // printf("middle = %s\n", middle);
    }
    else
    {
        // here we do have a filepath at the end and test points to the beginning of this
        // filepath, i.e. the first '/' of the path

        filepath = result;

        test = url;

        do
        {
            // printf("here\n");
            //find the last dot before the path. this immediately precedes the top level domain.
            test = strchr(&(test[1]),'.');
            // printf("test = %s\n", test);
        }
        while((strchr(&(test[1]),'.') - filepath < 0) && (strchr(&(test[1]),'.') != NULL));

        top_level_domain = &(test[1]);

        // printf("tld = %s\n", top_level_domain);

        // find the tld length
        tld_size = filepath - top_level_domain;

        test = url;

        do
        {   
            //find the second last dot. this is what seperates the hostname
            //from the remainder of the host
            test = strchr(&(test[1]),'.');
        }while( (strchr(&(strchr(&(test[1]),'.')[1]),'.') - filepath < 0) && (strchr(&(strchr(&(test[1]),'.')[1]),'.') != NULL));

        middle = test;

        // printf("middle = %s\n",middle );
        middle_size = top_level_domain - middle;
    }

    test = url;
    do 
    {
        test = strchr(test + sizeof(char),'/');
        // printf("test = %s\n", test);
        // sleep(1);
    }while (test[-1] != '/');

    host = test + sizeof(char);

    // printf("host = %s\n",host );

    hostname_size = middle - host;

    // printf("hostname_size = %d, tld_size = %d, middle_size = %d\n", hostname_size,tld_size,middle_size);

    strncpy(new_host, top_level_domain, tld_size);
    // printf("new_host = %s\n", new_host );
    strncpy(new_host + sizeof(char)*tld_size,middle,middle_size);
    // printf("new_host = %s\n", new_host );
    strncpy(new_host + sizeof(char)*(tld_size+middle_size),host,hostname_size);
    // printf("new_host = %s\n", new_host );

    if(result !=NULL)
    {
        strcpy(new_host + sizeof(char)*(tld_size+middle_size+hostname_size), filepath);
        // printf("new_host = %s\n", new_host );
    }

    // printf("%s\n",test);


    

    return new_host;

}