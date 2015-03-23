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
void filter_and_store(char * raw_links, char * output);
char * get_next_link(char * from_file, char ** output_pointer, char * parsed);
void add_edge(struct Linked_list * a, int from_index, int to_index);
void add_link(struct Linked_list * a, char * hyperlink, char * filtered_hyperlink, int index);
// void remove_file(char * to_remove);

int total_hyperlinks = 1;
int link_under_wget = 1;
int switcher = 0;
struct Linked_list parsed_links;
struct Linked_list parse_queue;

int main()
{
    char * next_hyperlink; 
    int i = 0;
    char cwd[1024];

    char ** input_files = (char **)malloc(2*sizeof(char *));
    char filename1[20] = "input1.txt";
    char filename2[20] = "input2.txt";
    char raw_links[20] = "raw_links.txt";
    char parsed_links[20] = "parsed.txt";
    char filtered_links[20] = "rx_queue.txt";
    input_files[0] = filename1;
    input_files[1] = filename2; 
    
    // get the graph starteds
    construct_2DArray(&graph);

    // clear all the files that will be used
    i = remove(append_directory(input_files[0]));
    i = remove(append_directory(input_files[1]));
    i = remove(append_directory(raw_links));
    i = remove(append_directory(parsed_links));
    i = remove(append_directory(filtered_links));
    
    
    char output[200];   // holds the most recently obtained hyperlink
    char starter[] = "http://www.mcgill.ca";    // the starting node

    //--------------------------------------------------------------------------
    // STEP 1

    //open the output file
    FILE * ofp = fopen(parsed_links,"w");

    // print to the output file so that we know that we've seen it before
    fprintf(ofp, "%s %s %d\n",add_slash(remove_extra(flip_url(starter))),remove_extra(starter),link_under_wget);


    //----------------------------------------------------------------------------
    //STEP 2 + 3
    // get all the hyperlinks from the starter node and print them to the temporary raw_links file
    wget_wrapper(starter, append_directory(raw_links));

    //------------------------------------------------------------------------------
    // STEP 4
    filter_and_store(append_directory(raw_links), append_directory(filtered_links));

    //-----------------------------------------------------------------------------
    // STEPS 5,6,7
    next_hyperlink = get_next_link(append_directory(filtered_links),input_files, append_directory(parsed_links));
    link_under_wget++;

    //---------------------------------------------------------------------------
    // STEP 8
    fprintf(ofp, "%s %s %d\n", add_slash(remove_extra(flip_url(next_hyperlink))),
        remove_extra(next_hyperlink),link_under_wget);    

    // STEP 9
    switcher = abs(1-switcher);

    return 0;

}



// void remove_file(char * to_remove)
// {

//     char * command = (char *)malloc(sizeof(char)*(5 + strlen(to_remove)));
//     strcat(command,"rm ");
//     printf("command = %s\n", command);    
//     strncpy(command + sizeof(char)*3, to_remove, strlen(to_remove));
//     printf("command = %s\n", command);
//     system(command);
//     free(command);
// }
void add_edge(struct Linked_list * a, int from_index, int to_index)
{
    struct node * temp = a->root;


    do
    {
        if(temp->data == from_index)
        {
            if ((temp->edges)->size == 0)
            {
                construct_Array(temp->edges);
            }

            (temp->edges)->size += 1;
            (temp->edges)->array = (int *)realloc((temp->edges)->array, sizeof(int)*((temp->edges)->size));
            ((temp->edges)->array)[(temp->edges)->size - 1] = to_index;
            break;
        }

        temp = temp->next;
    }while(temp != a->root);
}

void add_link(struct Linked_list * a, char * hyperlink, char * filtered_hyperlink, int index)
{
    add_node(a);
    ((a->root)->previous)->hyperlink = hyperlink;
    ((a->root)->previous)->filtered_hyperlink = filtered_hyperlink;
    ((a->root)->previous)->data = index;
}


void filter_and_store(char * raw_links, char * output)
{
    // step 4
    

    FILE * ifp = fopen(raw_links,"r");
    printf("here\n");
    FILE * ofp = fopen(output,"a");
    char * link_to_filter;


    while(fscanf(ifp,"%s", link_to_filter) == 1)
    {
        fprintf(ofp, "%s %s %d\n", add_slash(flip_url(remove_extra(link_to_filter))), 
            remove_extra(link_to_filter), link_under_wget);
    }; 

    fclose(ifp);
    fclose(ofp);
}

char * get_next_link(char * from_file, char ** output_pointer, char * parsed)
{   

    // step 5, 6, and 7

    char * next_link;
    char * completely_filtered, * link;
    char * test, * dummy; 
    int anchor_index, dummy_anchor_index, possible_index, found, next_link_found = 0;
    FILE * ifp = fopen(from_file,"r");
    FILE * ofp = fopen(append_directory(output_pointer[switcher]),"a");
    FILE * checker = fopen(append_directory(output_pointer[1-switcher]),"r");
    FILE * parsed_links = fopen(append_directory(parsed),"r");

    //get the next link in the input file (the filtered links)
    while(fscanf(ifp,"%s %s %d\n",completely_filtered,link,&anchor_index) == 3)
    {
        found = 0;  // test variable to see if we've found a match in the previous
        while(fscanf(checker,"%s %s %d %d\n", test,dummy,&dummy_anchor_index,&possible_index) == 4)
        {
            //search through the other file of the links that resulted from the previous
            // wgets to see if this link already exists in that file and then give it that number
            if(strcmp(test,completely_filtered) == 0)
            {   
                fprintf(ofp, "%s %s %d %d\n", completely_filtered,link,anchor_index,possible_index);
                found = 1;
                break;
            }
        };

        // if we haven't found the link in the other file that we're switching back and forth from
        // it might exists in the parsed file of links that have already been crawled
        // remember that the format for this file is (completely filtered, extra removed, this page's index)
        while((found == 0) && (fscanf(parsed_links,"%s %s %d", test, dummy, &possible_index) == 3))
        {
           if(strcmp(test,completely_filtered) == 0)
            {   
                fprintf(ofp, "%s %s %d %d\n", completely_filtered,link,anchor_index,possible_index);
                found = 1;
                break;
            } 
        };

        // if after all that checking we have yet to find a matching link, we have a new link
        // and we supply it with a new index number. Additionally, if this is the first new
        // link that we've found then this is next link that we want to crawl
        if (found == 0)
        {
            total_hyperlinks++;
            possible_index = total_hyperlinks;
            fprintf(ofp, "%s %s %d %d\n", completely_filtered,link,anchor_index,possible_index);

            if(next_link_found == 0)
            {
                next_link = link;
                next_link_found == 1;
            }
        }

        // move back to the top of the checker file to test again with the next link
        rewind(checker);

        // add the edge between the anchor link and each link pointed to by the anchor
        add_edge(anchor_index,possible_index);
    };

    fclose(ifp);
    fclose(ofp);
    fclose(checker);
    fclose(parsed_links);

    return next_link;
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
    char * result;
    int total_size;

    test = strchr(url,'?');
    // printf("test = %s\n", test );

    if (test != NULL)
    {
        total_size = test-url;
        url[total_size] = '\0';
    }

    return url;
}

void wget_wrapper(char * website, char * outputfile)
{
    char * link_to_filter;

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
    system(final_wget);

}



// void crawl_page(char * inputfile, char * newinputfile, char * parsed_links, char * raw_links)
// {

//     int i;
//     char * flipped, start[200];
//     int exists = 0;
    

//     FILE * ifp = fopen(inputfile,"r"); //where we will retreive the next hyperlink to be scoured
//     FILE * ofp = fopen(newinputfile,"w"); //where we will store the remainder of the hyperlinks in the file
//     FILE * parsed = fopen(parsed_links,"r");

//     char * output;  //this will hold the first hyperlink in the inputfile
//     char dummy[200], test[200];

//     //get the first hyperlink
//     fscanf(ifp,"%s", start);
//     // printf("start = %s, length = %d\n", start, (int)(strlen(start)) );
//     output = (char *)malloc(sizeof(char)*strlen(start));
//     output = remove_extra(start);

//     // printf("output = %s\n", output );

//     output[strlen(start)] = '\0';
//     // printf("output before flipped = %s...TEST\n", output );

//     //get the link as how it would appear in the final file
//     flipped = flip_url(output);
//     // printf("output after flipped = %s...TEST\n", output );
//     flipped = add_slash(flipped);

//     //store the remainder of the hyperlinks in the new input file;
//     while(fscanf(ifp,"%s",test) == 1)
//     {
//         // printf("output = %s\n",output );
//         fprintf(ofp, "%s\n", test);

//     };

//     fclose(ifp);
//     fclose(ofp);

//     // use the parsed_links, which contains a list of already-crawled pages, as an input
//     ifp = fopen(parsed_links,"r");

//     // search through the output file to determine if the link has already been used before
//     while(fscanf(ifp,"%s %s %d", test,dummy, &i) == 3)
//     {   
//         if (strcmp(flipped,test) == 0)
//         {
//             exists = 1;
//         }
//     };



//     // if it hasn't been used before, append it to the list
//     if (exists == 0)
//     {
//         ofp = fopen(outputfile,"a");

//         fprintf(ofp, "%s %s %d\n", flipped, output, total_hyperlinks++);

//         fclose(ofp);

        

//         // add all the links we see while going through this link
//         wget_wrapper(output,newinputfile);

//     }

// }

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

    char * host, * top_level_domain, * end_of_string, * middle, * filepath, * result, * hostname;   
    int tld_size, last_letter_of_host, i, middle_size, hostname_size;

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