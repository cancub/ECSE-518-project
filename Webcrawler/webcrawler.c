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
void filter_and_store(struct Linked_list * a, char * raw_links);
char * get_next_link(struct Linked_list * queue, struct Linked_list * parsed);
void add_edge(struct Linked_list * a, int from_index, int to_index);
void add_link(struct Linked_list * a, char * link, int index);
char * clean_link(char * url);
int search_for_link(struct Linked_list * a, struct node * n);
// void remove_file(char * to_remove);

int total_parsed_hyperlinks = 1;
int index_under_wget = 1;

int main()
{
    char * link; 
    char raw_links[] = "raw_links.txt";
    struct Linked_list * parsed_links;
    struct Linked_list * parse_queue;

    char starter[] = "http://www.mcgill.ca";    // the starting node

    //initialize linked lists
    parsed_links = initialize_linked_list(1);
    parse_queue = initialize_linked_list(1);

    link = starter;
    //--------------------------------------------------------------------------
    // STEP 1
    // print to the output file so that we know that we've seen it before

    add_link(parsed_links,link ,total_parsed_hyperlinks++);

    do
    {
        //----------------------------------------------------------------------------
        //STEP 2 + 3
        // get all the hyperlinks from the starter node and print them to the temporary raw_links file
        wget_wrapper(link, raw_links);


        //------------------------------------------------------------------------------
        // STEP 4
        // parse the newest links to see if there are any duplicates, and 
        // format the links in two ways such that they can be easily used AND compared.
        // finally, add an index to the link to show which page linked to it.
        filter_and_store(parse_queue, raw_links);

        //-----------------------------------------------------------------------------
        // STEPS 5,6,7
        // 
        link = get_next_link(parse_queue, parsed_links);
        // printf("Current queue:\n");
        // print_linked_list(parse_queue);
        // printf("\n");
        // printf("Parsed links:\n");
        // print_linked_list(parsed_links);
        // printf("\n");
        index_under_wget++;

        printf("index under wget = %d\n",index_under_wget );

    }while(index_under_wget < 35);



    printf("Current parsed list:\n");
    print_linked_list(parsed_links);
    printf("\n");
    printf("Current queue:\n");
    print_linked_list(parse_queue);

    delete_linked_list(parse_queue);
    delete_linked_list(parsed_links);

    return 0;

}



//
//functions

int search_for_link(struct Linked_list * a, struct node * n)
{

    struct node * test;
    test = a->root;

    // printf("testing = %s\n", n->filtered_hyperlink);

    // print_linked_list(a);

    if (a->size > 0)
    {
        // cycle through each of the links in the linked list to compare with this
        // potential node n.

        if (index_under_wget > 1)
        {
            // printf("%s\n", n->filtered_hyperlink);
        }
        do  
        {              

            // if the test link is the same as that of n, we need to go further, partly
            // for differentiation of whether this operation is being performed
            // for the parsed links list or the queue.

            if ( strcmp(test->filtered_hyperlink,n->filtered_hyperlink) == 0)
            {

                // printf("*************found %s\n",n->hyperlink );

                // send back the index of the link that matched n's. from this,
                // any calling function for the parsing queue will be able to cpompare to
                // the current index of the link under wget and see if they match.
                // on the other hand, when attmepting to add this link to the parsed list
                // literally any value other than being returned will give calling function
                // reason to not add the link

                // sleep(3);

                return test->data;

                // }
            }
            to_prev(&test);
            // printf("test == a->root? %d\n",test == a->root);
        }while(test != a->root);
    }

    // printf("\n");

    return 0;
}

char * clean_link(char * url)
{
    return add_slash(remove_extra(flip_url(url)));
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

    // printf("printing edges of %d, whose array has size %d\n",temp->data, (int)(temp->edges.size));
    // print_Array(temp->edges);
}

void add_link(struct Linked_list * a, char * link, int index)
{

    // as the inbound links from wget are being filtered, a link will be added
    // to the crawling queue if it does not already exist there for the same
    // reference page. If this is not the case, then it is added with its data
    // element representing the index of the page that is linking to it.


    // filter and copy the input information (linking page index, wget-able hyperlink
    // and a fitlered hpyerlink for sorting) into a new node

    struct node * temp = (struct node *)malloc(sizeof(struct node));
    char * new_link = remove_extra(link);
    char * filtered_link = clean_link(link);

    temp->hyperlink =(char *)malloc(strlen(new_link) + 1);
    strcpy(temp->hyperlink, new_link);
    temp->filtered_hyperlink = (char *)malloc(strlen(filtered_link) + 1);
    strcpy(temp->filtered_hyperlink, filtered_link);

    temp->data = index;
    construct_Array(&(temp->edges));

    // sleep(1);

    // printf("index = %d, index_under_wget = %d\n", index, index_under_wget);

    // if the link does not already exist for the same linking page, add it to
    // the list
    if(search_for_link(a,temp) != index_under_wget)
    {
        append_node(a,temp);
    }
}


void filter_and_store(struct Linked_list * a, char * raw_links)
{
    // step 4  
    // we create a new linked list to hold the new links, add the links to this
    // new list while rejecting duplicates for this list and this list alone
    // and then append this listto the end of a

    struct Linked_list * temp;
    int start_size = a->size;

    if(start_size != 0)
    {
        temp = initialize_linked_list(1);
    }
    else
    {
        temp = a;
    }
        
    FILE * ifp = fopen(raw_links,"r");
    char link_to_filter[200];

    if(ifp == NULL)
    {
        printf("Could not open %s for writing\n", raw_links);
        exit(0);
    }

    while(fscanf(ifp,"%s", link_to_filter) == 1)
    {
        // for the time being, we use the data field of the node for the index of
        // the link POINTING TO this link. this will be changed when we move this
        // link to the parsed list

        // printf("unfiltered = %s, filtered = %s\n",link_to_filter, remove_extra(link_to_filter));
        add_link(temp,link_to_filter, index_under_wget);
        // printf("Current queue:\n");
        // print_linked_list(a);
        // printf("\n");
        // sleep(1);
    }; 

    fclose(ifp);


    // printf("\nprinting old queue of size %d prior to new adittions\n", (int)(a->size));
    // print_linked_list(a);
    // printf("\nprinting new additions of size %d prior to concat\n", (int)(temp->size));
    // print_linked_list(temp);

    if(start_size != 0)
    {
        concatenate_lists(a,temp);
    }
}

char * get_next_link(struct Linked_list * queue, struct Linked_list * parsed)
{   

    // step 5, 6, and 7
    struct node * parsed_test;
    int found;

    do
    {   
        // remove the first link in the queue for testing
        parsed_test = pop_node(queue, 0);

        // printf("searching for %s\n",parsed_test->filtered_hyperlink );
        found = search_for_link(parsed, parsed_test);
        // printf("found = %d\n",found);
        // printf("\n");

        if (found != 0)
        {
            // if we can locate this link amongst those that have already been parsed
            // then we do nothing but add this edge to the link that pointed to this link
            add_edge(parsed,parsed_test->data,found);
        }
        else
        {
            // otherwise, we know that search_for_link will return a 0 if the link we are
            // looking at has yet to be parsed. thus, we have found a new link and we should
            // assign to it a new index. Additionally, we still want to add the edge

            // add edge to the pre-existing node (it has already been added or else
            // we wouldn't be looking at a page it links to)
            add_edge(parsed,parsed_test->data, total_parsed_hyperlinks);
            // assign an index to this page and inciment the number of links
            parsed_test->data = total_parsed_hyperlinks++;
            // add this page to the parsed list
            append_node(parsed,parsed_test);

            return parsed_test->hyperlink;
        }

    }while(parsed_test != queue->root);

    return "error";
}


char * append_directory(char * filename)
{
    // printf("%s\n", filename);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    // printf("length of cwd = %d, string length of cwd = %d\n",(int)(sizeof(cwd)), (int)(strlen(cwd)) );
    char * result = (char *)malloc(sizeof(char)*(strlen(cwd) + strlen(filename)));
    strcat(result,cwd);
    strcat(result,"/text_files/");
    strcat(result,filename);
    // printf("result = %s\n", result);
    return result;
}


char * remove_extra(char * url)
{
    char * test = url;
    int total_size;

    test = strchr(url,'?');
    // printf("test = %s\n", test );

    if (test != NULL)
    {
        // printf("before = %s\n",url);
        total_size = test-url;
        url[total_size] = '\0';
        // printf("after = %s\n",url);
    }

    return url;
}

void wget_wrapper(char * website, char * outputfile)
{

    // this is the template for the wget, we replace the x with the url we want to
    // crawl and the y at the very end with the location of the file we are dumping the
    // raw urls into
    char web_address[BUFFLEN] = "wget -q x -O - | tr \"\\t\\r\\n'\" '   \"' | grep -i -o '\
<a[^>]\\+href[ ]*=[ \\t]*\"\\(ht\\|f\\)tps\\?:[^\"]\\+\"' |\
sed -e 's/^.*\"\\([^\"]\\+\\)\".*$/\\1/g' > y"; 

    // find the relative locations of x an y in the template to determine where to splice
    // in the url and dump file
    char * after_address = strchr(web_address,'x');
    int first_copy = after_address - web_address;
    after_address += sizeof(char);
    char * after_file = strchr(web_address,'y');
    int second_copy = after_file - after_address;
    after_file += sizeof(char); 

    char final_wget[200];

    // build the full wget command
    int length = 0;
    strncpy(final_wget + length*sizeof(char), web_address, first_copy);
    length += first_copy;
    strncpy(final_wget + length*sizeof(char), website,strlen(website));
    length += strlen(website);
    strncpy(final_wget + length*sizeof(char), after_address, second_copy);
    length += second_copy;
    strncpy(final_wget + length*sizeof(char), outputfile, strlen(outputfile));
    length += strlen(outputfile);
    strcpy(final_wget + length*sizeof(char), after_file);

    // call the wget
    // printf("%s\n",final_wget );
    system(final_wget);

}

char * add_slash(char * url)
{
    char * test = url;
    char * result = (char *)malloc(sizeof(char) * (strlen(url) + 2));

    int i = 0;

    while(test[i] != '\0')
    {
        i++;
    };

    if (test[i-1] != '/')
    {
        strncpy(result,url,strlen(url));
        result[strlen(url)]='/';
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

    char * new_host = (char *)malloc(length*sizeof(char));
    


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